#ifndef SYNCDOCUMENT_H
#define SYNCDOCUMENT_H

extern "C" {
#include "../lib/data.h"
}

#include <QStack>
#include <QList>
#include <QVector>
#include <QString>

#include "clientsocket.h"

class SyncDocument : public sync_data {
public:
	SyncDocument() :
	    rows(128), savePointDelta(0), savePointUnreachable(false)
	{
		this->tracks = NULL;
		this->num_tracks = 0;
	}

	~SyncDocument();

	size_t createTrack(const QString &name)
	{
		size_t index = sync_create_track(this, name.toUtf8());
		trackOrder.push_back(index);
		return index;
	}

	class Command
	{
	public:
		virtual ~Command() {}
		virtual void exec(SyncDocument *data) = 0;
		virtual void undo(SyncDocument *data) = 0;
	};
	
	class InsertCommand : public Command
	{
	public:
		InsertCommand(int track, const track_key &key) : track(track), key(key) {}
		~InsertCommand() {}
		
		void exec(SyncDocument *data)
		{
			sync_track *t = data->tracks[track];
			Q_ASSERT(!is_key_frame(t, key.row));
			if (sync_set_key(t, &key))
				throw std::bad_alloc();
			data->clientSocket.sendSetKeyCommand(t->name, key); // update clients
		}
		
		void undo(SyncDocument *data)
		{
			sync_track *t = data->tracks[track];
			Q_ASSERT(is_key_frame(t, key.row));
			if (sync_del_key(t, key.row))
				throw std::bad_alloc();
			data->clientSocket.sendDeleteKeyCommand(t->name, key.row); // update clients
		}

	private:
		int track;
		track_key key;
	};
	
	class DeleteCommand : public Command
	{
	public:
		DeleteCommand(int track, int row) : track(track), row(row) {}
		~DeleteCommand() {}
		
		void exec(SyncDocument *data)
		{
			sync_track *t = data->tracks[track];
			int idx = sync_find_key(t, row);
			Q_ASSERT(idx >= 0);
			oldKey = t->keys[idx];
			if (sync_del_key(t, row))
				throw std::bad_alloc();
			data->clientSocket.sendDeleteKeyCommand(t->name, row); // update clients
		}
		
		void undo(SyncDocument *data)
		{
			sync_track *t = data->tracks[track];
			Q_ASSERT(!is_key_frame(t, row));
			if (sync_set_key(t, &oldKey))
				throw std::bad_alloc();
			data->clientSocket.sendSetKeyCommand(t->name, oldKey); // update clients
		}

	private:
		int track, row;
		struct track_key oldKey;
	};

	
	class EditCommand : public Command
	{
	public:
		EditCommand(int track, const track_key &key) : track(track), key(key) {}
		~EditCommand() {}

		void exec(SyncDocument *data)
		{
			sync_track *t = data->tracks[track];
			int idx = sync_find_key(t, key.row);
			Q_ASSERT(idx >= 0);
			oldKey = t->keys[idx];
			if (sync_set_key(t, &key))
				throw std::bad_alloc();
			data->clientSocket.sendSetKeyCommand(t->name, key); // update clients
		}

		void undo(SyncDocument *data)
		{
			sync_track *t = data->tracks[track];
			Q_ASSERT(is_key_frame(t, key.row));
			if (sync_set_key(t, &oldKey))
				throw std::bad_alloc();
			data->clientSocket.sendSetKeyCommand(t->name, oldKey); // update clients
		}
		
	private:
		int track;
		track_key oldKey, key;
	};
	
	class MultiCommand : public Command
	{
	public:
		~MultiCommand()
		{
			while (!commands.isEmpty())
				delete commands.takeFirst();
		}
		
		void addCommand(Command *cmd)
		{
			commands.push_back(cmd);
		}
		
		size_t getSize() const { return commands.size(); }
		
		void exec(SyncDocument *data)
		{
			QListIterator<Command *> i(commands);
			while (i.hasNext())
				i.next()->exec(data);
		}
		
		void undo(SyncDocument *data)
		{
			QListIterator<Command *> i(commands);
			i.toBack();
			while (i.hasPrevious())
				i.previous()->undo(data);
		}
		
	private:
		QList<Command*> commands;
	};
	
	void exec(Command *cmd)
	{
		undoStack.push(cmd);
		cmd->exec(this);
		clearRedoStack();

		if (savePointDelta < 0) savePointUnreachable = true;
		savePointDelta++;
	}
	
	bool undo()
	{
		if (undoStack.size() == 0) return false;
		
		Command *cmd = undoStack.top();
		undoStack.pop();
		
		redoStack.push(cmd);
		cmd->undo(this);
		
		savePointDelta--;
		
		return true;
	}
	
	bool redo()
	{
		if (redoStack.size() == 0) return false;
		
		Command *cmd = redoStack.top();
		redoStack.pop();
		
		undoStack.push(cmd);
		cmd->exec(this);

		savePointDelta++;

		return true;
	}
	
	void clearUndoStack()
	{
		while (!undoStack.empty())
		{
			Command *cmd = undoStack.top();
			undoStack.pop();
			delete cmd;
		}
	}
	
	void clearRedoStack()
	{
		while (!redoStack.empty())
		{
			Command *cmd = redoStack.top();
			redoStack.pop();
			delete cmd;
		}
	}
	
	Command *getSetKeyFrameCommand(int track, const track_key &key)
	{
		sync_track *t = tracks[track];
		SyncDocument::Command *cmd;
		if (is_key_frame(t, key.row)) cmd = new EditCommand(track, key);
		else                          cmd = new InsertCommand(track, key);
		return cmd;
	}

	size_t getTrackCount() const
	{
		return trackOrder.size();
	}
	
	size_t getTrackIndexFromPos(size_t track) const
	{
		Q_ASSERT(track < (size_t)trackOrder.size());
		return trackOrder[track];
	}

	void swapTrackOrder(size_t t1, size_t t2)
	{
		Q_ASSERT(t1 < (size_t)trackOrder.size());
		Q_ASSERT(t2 < (size_t)trackOrder.size());
		std::swap(trackOrder[t1], trackOrder[t2]);
	}

	static SyncDocument *load(const QString &fileName);
	bool save(const QString &fileName);

	bool modified() const
	{
		if (savePointUnreachable) return true;
		return 0 != savePointDelta;
	}

	bool isRowBookmark(int row) const
	{
		QList<int>::const_iterator it = qLowerBound(rowBookmarks.begin(), rowBookmarks.end(), row);
		return it != rowBookmarks.end() && *it == row;
	}

	void toggleRowBookmark(int row)
	{
		QList<int>::iterator it = qLowerBound(rowBookmarks.begin(), rowBookmarks.end(), row);
		if (it == rowBookmarks.end() || *it != row)
			rowBookmarks.insert(it, row);
		else
			rowBookmarks.erase(it);
	}

	ClientSocket clientSocket;

	size_t getRows() const { return rows; }
	void setRows(size_t rows) { this->rows = rows; }

	QString fileName;

	int nextRowBookmark(int row) const
	{
		QList<int>::const_iterator it = qUpperBound(rowBookmarks.begin(), rowBookmarks.end(), row);
		if (it == rowBookmarks.end())
			return -1;
		return *it;
	}

	int prevRowBookmark(int row) const
	{
		QList<int>::const_iterator it = qLowerBound(rowBookmarks.begin(), rowBookmarks.end(), row);
		if (it == rowBookmarks.end()) {

			// this can only really happen if the list is empty
			if (it == rowBookmarks.begin())
				return -1;

			// reached the end, pick the last bookmark if it's after the current row
			it--;
			return *it < row ? *it : -1;
		}

		// pick the previous key (if any)
		return it != rowBookmarks.begin() ? *(--it) : -1;
	}

private:
	QList<int> rowBookmarks;
	QVector<size_t> trackOrder;
	size_t rows;

	// undo / redo functionality
	QStack<Command*> undoStack;
	QStack<Command*> redoStack;
	int savePointDelta;        // how many undos must be done to get to the last saved state
	bool savePointUnreachable; // is the save-point reachable?

};

#endif // !defined(SYNCDOCUMENT_H)
