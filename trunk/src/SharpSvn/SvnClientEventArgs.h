// $Id$
// Copyright (c) SharpSvn Project 2007 
// The Sourcecode of this project is available under the Apache 2.0 license
// Please read the SharpSvnLicense.txt file for more details

#pragma once

#include "SvnEnums.h"
#include "SvnCommitItem.h"
#include "svn_compat.h"

namespace SharpSvn {
	using System::Collections::Generic::ICollection;
	using System::Collections::Generic::IDictionary;
	using System::Collections::Generic::IList;
	using System::Collections::Generic::SortedList;
	using System::Collections::ObjectModel::KeyedCollection;

	ref class SvnException;
	ref class SvnMergeRange;

	public ref class SvnEventArgs abstract : public System::EventArgs
	{
	protected:
		SvnEventArgs()
		{
		}

	public:
		/// <summary>Detaches the SvnEventArgs from the unmanaged storage; optionally keeping the property values for later use</summary>
		/// <description>After this method is called all properties are either stored managed, or are no longer readable</description>
		virtual void Detach(bool keepProperties)
		{
			UNUSED_ALWAYS(keepProperties);
		}
	};
};

#include "SvnClientEventArgs_Global.h"
#include "SvnClientEventArgs_Status.h"
	
namespace SharpSvn {
	
	public ref class SvnNotifyEventArgs sealed : public SvnEventArgs
	{
		const svn_wc_notify_t *_notify;
		initonly SvnNotifyAction _action;
		initonly SvnNodeKind _nodeKind;
		initonly SvnNotifyState _contentState;
		initonly SvnNotifyState _propertyState;
		initonly SvnLockState _lockState;
		initonly __int64 _revision;
		SvnLockInfo^ _lock;
		String^ _changelistName;
		SvnMergeRange^ _mergeRange;
		bool _pathIsUri;
		bool _mimeTypeIsBinary;

	internal:
		SvnNotifyEventArgs(const svn_wc_notify_t *notify)
		{
			if(!notify)
				throw gcnew ArgumentNullException("notify");

			_notify = notify;
			_action = (SvnNotifyAction)notify->action;
			_nodeKind = (SvnNodeKind)notify->kind;
			_contentState = (SvnNotifyState)notify->content_state;
			_propertyState = (SvnNotifyState)notify->prop_state;
			_lockState = (SvnLockState)notify->lock_state;
			_revision = notify->revision;
		}

	private:
		String^ _fullPath;
		String^ _path;
		String^ _mimeType;
		SvnException^ _exception;		

	public:
		/// <summary>The path the notification is about</summary>
		/// <remarks>The <see cref="FullPath" /> property contains the path in normalized format; while <see cref="Path" /> returns the exact path from the subversion api</remarks>
		property String^ Path
		{
			String^ get()
			{
				if(!_path && _notify && _notify->path)
				{
					_pathIsUri = (0 != svn_path_is_url(_notify->path));
					_path = SvnBase::Utf8_PtrToString(_notify->path);

					if(!_pathIsUri && _path)
						_path = _path->Replace('/', System::IO::Path::DirectorySeparatorChar);
				}

				return _path;
			}
		}

		/// <summary>The path the notification is about, translated via <see cref="System::IO::Path::GetFullPath" /></summary>
		/// <remarks>The <see cref="FullPath" /> property contains the path in normalized format; while <see cref="Path" /> returns the exact path from the subversion api</remarks>
		property String^ FullPath
		{
			String^ get()
			{
				if(!_fullPath && Path)
					_fullPath = _pathIsUri ? Path : System::IO::Path::GetFullPath(Path);

				return _fullPath;
			}
		}

		/// <summary>Gets a boolean indicating whether the path is a Uri</summary>
		property bool PathIsUri
		{
			bool get()
			{
				GC::KeepAlive(Path);
				return _pathIsUri;
			}
		}

		property SvnNotifyAction Action
		{
			SvnNotifyAction get()
			{
				return _action;
			}
		}

		property SvnNodeKind NodeKind
		{
			SvnNodeKind get()
			{
				return _nodeKind;
			}
		}

		property String^ MimeType
		{
			String^ get()
			{
				if(!_mimeType && _notify && _notify->mime_type)
				{
					_mimeType = SvnBase::Utf8_PtrToString(_notify->mime_type);
					_mimeTypeIsBinary = (0 != svn_mime_type_is_binary(_notify->mime_type));
				}

				return _mimeType;
			}
		}

		property bool MimeTypeIsBinary
		{
			bool get()
			{
				GC::KeepAlive(MimeType);
				return _mimeTypeIsBinary;
			}
		}

		property SvnException^ Error
		{
			SvnException^ get();
		}

		property SvnNotifyState ContentState
		{
			SvnNotifyState get()
			{
				return _contentState;
			}
		}

		property SvnNotifyState PropertyState
		{
			SvnNotifyState get()
			{
				return _propertyState;
			}
		}

		property SvnLockState LockState
		{
			SvnLockState get()
			{
				return _lockState;
			}
		}

		property __int64 Revision
		{
			__int64 get()
			{
				return _revision;
			}
		}

		property SvnLockInfo^ Lock
		{
			SvnLockInfo^ get()
			{
				if(!_lock && _notify && _notify->lock)
					_lock = gcnew SvnLockInfo(_notify->lock, false);

				return _lock;
			}
		}

		property String^ ChangeListName
		{
			String^ get()
			{
				if(!_changelistName && _notify && _notify->changelist_name)
					_changelistName = SvnBase::Utf8_PtrToString(_notify->changelist_name);

				return _changelistName;
			}
		}

		property SvnMergeRange^ MergeRange
		{
			SvnMergeRange^ get();
		}

	public:
		/// <summary>Detaches the SvnEventArgs from the unmanaged storage; optionally keeping the property values for later use</summary>
		/// <description>After this method is called all properties are either stored managed, or are no longer readable</description>
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if(keepProperties)
				{
					// Use all properties to get them cached in .Net memory
					GC::KeepAlive(Path);
					GC::KeepAlive(MimeType);
					GC::KeepAlive(Error);
					GC::KeepAlive(Lock);
					GC::KeepAlive(ChangeListName);
					GC::KeepAlive(MergeRange);
				}

				if(_lock)
					_lock->Detach(keepProperties);
			}
			finally
			{
				_notify = nullptr;
				__super::Detach(keepProperties);
			}
		}
	};

	ref class SvnClientArgs;	

	public ref class SvnProcessingEventArgs sealed : public SvnEventArgs
	{
		initonly SvnClientCommandType _commandType;

	internal:
		SvnProcessingEventArgs(SvnClientCommandType commandType)
		{
			EnumVerifier::Verify(commandType);
			
			_commandType = commandType;
		}

	internal:
		property SvnClientCommandType CommandType
		{
			SvnClientCommandType get()
			{
				return _commandType;
			}
		}
	};

	public ref class SvnChangeItem sealed
	{
		initonly String^ _path;
		initonly SvnChangeAction _action;
		initonly String^ _copyFromPath;
		initonly __int64 _copyFromRevision;
	internal:
		SvnChangeItem(String^ path, SvnChangeAction action, String^ copyFromPath, __int64 copyFromRevision)
		{
			if(String::IsNullOrEmpty(path))
				throw gcnew ArgumentNullException("path");

			_path = path;
			_action = action;
			_copyFromPath = copyFromPath;
			_copyFromRevision = copyFromPath ? copyFromRevision : -1;
		}

	public:
		property String^ Path
		{
			String^ get()
			{
				return _path;
			}
		}

		property SvnChangeAction Action
		{
			SvnChangeAction get()
			{
				return _action;
			}
		}

		property String^ CopyFromPath
		{
			String^ get()
			{
				return _copyFromPath;
			}
		}

		property __int64 CopyFromRevision
		{
			__int64 get()
			{
				return _copyFromRevision;
			}
		}
	};

	public ref class SvnLogEventArgs : public SvnCancelEventArgs
	{
		svn_log_entry_t* _entry;
		AprPool^ _pool;

		String^ _author;
		DateTime _date;
		String^ _message;
		__int64 _revision;
		bool _hasChildren;
		int _mergeLevel;

		const char* _pcAuthor;
		const char* _pcMessage;

	internal:
		SvnLogEventArgs(svn_log_entry_t *entry, int mergeLevel, AprPool ^pool)
		{
			if(!entry)
				throw gcnew ArgumentNullException("entry");
			else if(!pool)
				throw gcnew ArgumentNullException("pool");

			_entry = entry;
			_pool = pool;

			const char* pcAuthor = nullptr;
			const char* pcDate = nullptr;
			const char* pcMessage = nullptr;
			
			if(entry->revprops)
				svn_compat_log_revprops_out(&pcAuthor, &pcDate, &pcMessage, entry->revprops);


			if(pcDate)
			{
				apr_time_t when = 0; // Documentation: date must be parsable by svn_time_from_cstring()
				svn_error_t *err = pcDate ? svn_time_from_cstring(&when, pcDate, pool->Handle) : nullptr;

				if(!err)
					_date = SvnBase::DateTimeFromAprTime(when);
			}
			else
				_date = DateTime::MinValue;

			_revision = entry->revision;
			_pcAuthor = pcAuthor;
			_pcMessage = pcMessage;
			_hasChildren = entry->has_children ? true : false;

			_mergeLevel = mergeLevel;
		}

	private:
		ref class ChangedPathsCollection sealed : System::Collections::ObjectModel::KeyedCollection<String^, SvnChangeItem^>
		{
		protected:
			virtual String^ GetKeyForItem(SvnChangeItem^ item) override
			{
				if(!item)
					throw gcnew ArgumentNullException("item");

				return item->Path;
			}
		};

		ChangedPathsCollection^ _changedPaths;
	public:

		property KeyedCollection<String^, SvnChangeItem^>^ ChangedPaths
		{
			KeyedCollection<String^, SvnChangeItem^>^  get()
			{
				if(!_changedPaths && _entry && _pool)
				{
					_changedPaths = gcnew ChangedPathsCollection();

					for (apr_hash_index_t* hi = apr_hash_first(_pool->Handle, _entry->changed_paths); hi; hi = apr_hash_next(hi))
					{
						const char* pKey;
						apr_ssize_t keyLen;
						svn_log_changed_path_t *pChangeInfo;

						apr_hash_this(hi, (const void**)&pKey, &keyLen, (void**)&pChangeInfo);

						SvnChangeItem^ ci = gcnew SvnChangeItem(SvnBase::Utf8_PtrToString(pKey, (int)keyLen),
							(SvnChangeAction)pChangeInfo->action,
							SvnBase::Utf8_PtrToString(pChangeInfo->copyfrom_path),
							pChangeInfo->copyfrom_rev);

						_changedPaths->Add(ci);
					}
				}

				return _changedPaths;
			}
		}

		property __int64 Revision
		{
			__int64 get()
			{
				return _revision;
			}
		}

		property String^ Author
		{
			String^ get()
			{
				if(!_author && _pcAuthor)
					_author = SvnBase::Utf8_PtrToString(_pcAuthor);

				return _author;
			}
		}

		property DateTime Time
		{
			DateTime get()
			{

				return _date;
			}
		}

		property String^ LogMessage
		{
			String^ get()
			{
				if(!_message && _pcMessage)
				{
					_message = SvnBase::Utf8_PtrToString(_pcMessage);

					if(_message)
					{
						// Subversion log messages always use \n newlines
						_message = _message->Replace("\n", Environment::NewLine);
					}
				}

				return _message;
			}
		}

		/// <summary>Set to true when the following items are merged-child items of this item.</summary>
		property bool HasChildren
		{
			bool get()
			{
				return _hasChildren;
			}
		}

		/// <summary>Gets the nesting level of the logs via merges</summary>
		property int MergeLogNestingLevel
		{
			int get()
			{
				return _mergeLevel;
			}
		}

	public:
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if(keepProperties)
				{
					// Use all properties to get them cached in .Net memory
					GC::KeepAlive(ChangedPaths);
					GC::KeepAlive(Author);
					GC::KeepAlive(LogMessage);
				}
			}
			finally
			{
				_entry = nullptr;
				_pool = nullptr;
				_pcMessage = nullptr;
				_pcAuthor = nullptr;
				__super::Detach(keepProperties);
			}
		}

	};

	public ref class SvnInfoEventArgs : public SvnCancelEventArgs
	{
		const svn_info_t* _info;
		initonly String^ _path;
		String^ _fullPath;
		Uri^ _uri;
		initonly __int64 _rev;
		initonly SvnNodeKind _nodeKind;
		Uri^ _reposRootUri;
		String^ _reposUuid;
		initonly __int64 _lastChangeRev;
		initonly DateTime _lastChangeDate;
		String^ _lastChangeAuthor;
		SvnLockInfo^ _lock;
		initonly bool _hasWcInfo;
		SvnSchedule _wcSchedule;
		Uri ^_copyFromUri;
		initonly __int64 _copyFromRev;
		initonly DateTime _contentTime;
		initonly DateTime _propertyTime;
		String^ _checksum;
		String^ _conflict_old;
		String^ _conflict_new;
		String^ _conflict_wrk;
		String^ _prejfile;
		String^ _changelist;
		initonly SvnDepth _depth;
		initonly __int64 _wcSize;
		initonly __int64 _size;

	internal:
		SvnInfoEventArgs(String^ path, const svn_info_t* info)
		{
			if(String::IsNullOrEmpty(path))
				throw gcnew ArgumentNullException("path");
			else if(!info)
				throw gcnew ArgumentNullException("info");

			_info = info;
			_path = path;
			_rev = info->rev;
			_nodeKind = (SvnNodeKind)info->kind;
			if(info->last_changed_author)
			{
				_lastChangeRev = info->last_changed_rev;
				_lastChangeDate = SvnBase::DateTimeFromAprTime(info->last_changed_date);
			}
			_hasWcInfo = info->has_wc_info != 0;
			_wcSchedule = (SvnSchedule)info->schedule;
			_copyFromRev = info->copyfrom_rev;
			if(_hasWcInfo)
			{
				_contentTime = SvnBase::DateTimeFromAprTime(info->text_time);
				_propertyTime = SvnBase::DateTimeFromAprTime(info->prop_time);
			}

			_depth = (SvnDepth)info->depth;

			if(info->size == (apr_size_t)-1)
				_size = -1;
			else
				_size = info->size;

			if(info->working_size == (apr_size_t)-1)
				_wcSize = -1;
			else
				_wcSize = info->working_size;
		}
	public:
		property String^ Path
		{
			String^ get()
			{
				return _path;
			}
		}

		/// <summary>The path the notification is about, translated via <see cref="System::IO::Path::GetFullPath" /></summary>
		/// <remarks>The <see cref="FullPath" /> property contains the path in normalized format; while <see cref="Path" /> returns the exact path from the subversion api</remarks>
		property String^ FullPath
		{
			String^ get()
			{
				if(!_fullPath && Path && HasLocalInfo)
					_fullPath = System::IO::Path::GetFullPath(Path);

				return _fullPath;
			}
		}

		property Uri^ Uri
		{
			System::Uri^ get()
			{
				if(!_uri && _info && _info->URL)
					_uri = SvnBase::Utf8_PtrToUri(_info->URL, _nodeKind);

				return _uri;
			}
		}

		property __int64 Revision
		{
			__int64 get()
			{
				return _rev;
			}
		}

		property SvnNodeKind NodeKind
		{
			SvnNodeKind get()
			{
				return _nodeKind;
			}
		}

		/// <summary>Gets the repository root Uri; ending in a '/' at the end</summary>
		/// <remarks>The unmanaged api does not add the '/' at the end, but this makes using <see cref="System::Uri" /> hard</remarks>
		property System::Uri^ RepositoryRoot
		{
			System::Uri^ get()
			{
				if(!_reposRootUri && _info && _info->repos_root_URL)
					_reposRootUri = SvnBase::Utf8_PtrToUri(_info->repos_root_URL, SvnNodeKind::Directory);

				return _reposRootUri;
			}
		}

		property Guid RepositoryId
		{
			Guid get()
			{
				if(!_reposUuid && _info && _info->repos_UUID)
					_reposUuid = SvnBase::Utf8_PtrToString(_info->repos_UUID);

				return _reposUuid ? Guid(_reposUuid) : Guid::Empty;
			}
		}

		property __int64 LastChangeRevision
		{
			__int64 get()
			{
				return _lastChangeRev;
			}
		}

		property DateTime LastChangeTime
		{
			DateTime get()
			{
				return _lastChangeDate;
			}
		}

		property String^ LastChangeAuthor
		{
			String^ get()
			{
				if(!_lastChangeAuthor && _info && _info->last_changed_author)
					_lastChangeAuthor = SvnBase::Utf8_PtrToString(_info->last_changed_author);

				return _lastChangeAuthor;
			}
		}

		property SvnLockInfo^ Lock
		{
			SvnLockInfo^ get()
			{
				if(!_lock && _info && _info->lock)
					_lock = gcnew SvnLockInfo(_info->lock, HasLocalInfo);

				return _lock;
			}
		}

		property bool HasLocalInfo
		{
			bool get()
			{
				return _hasWcInfo;
			}
		}

		property SvnSchedule Schedule
		{
			SvnSchedule get()
			{
				return HasLocalInfo ? _wcSchedule : SvnSchedule::Normal;
			}
		}

		property System::Uri^ CopyFromUri
		{
			System::Uri^ get()
			{
				if(!_copyFromUri && _info && _info->copyfrom_url && HasLocalInfo)
					_copyFromUri = SvnBase::Utf8_PtrToUri(_info->copyfrom_url, _nodeKind);

				return _copyFromUri;
			}
		}

		property __int64 CopyFromRev
		{
			__int64 get()
			{
				return _copyFromRev;
			}
		}

		property DateTime ContentTime
		{
			DateTime get()
			{
				return _contentTime;
			}
		}

		property DateTime PropertyTime
		{
			DateTime get()
			{
				return _propertyTime;
			}
		}

		property String^ Checksum
		{
			String^ get()
			{
				if(!_checksum && _info && _info->checksum && HasLocalInfo)
					_checksum = SvnBase::Utf8_PtrToString(_info->checksum);

				return _checksum;
			}
		}

		property String^ ConflictOld
		{
			String^ get()
			{
				if(!_conflict_old && _info && _info->conflict_old && HasLocalInfo)
					_conflict_old = SvnBase::Utf8_PtrToString(_info->conflict_old);

				return _conflict_old;
			}
		}

		property String^ ConflictNew
		{
			String^ get()
			{
				if(!_conflict_new && _info && _info->conflict_new && HasLocalInfo)
					_conflict_new = SvnBase::Utf8_PtrToString(_info->conflict_new);

				return _conflict_new;
			}
		}

		property String^ ConflictWork
		{
			String^ get()
			{
				if(!_conflict_wrk && _info && _info->conflict_wrk && HasLocalInfo)
					_conflict_wrk = SvnBase::Utf8_PtrToString(_info->conflict_wrk);

				return _conflict_wrk;
			}
		}

		property String^ PropertyEditFile
		{
			String^ get()
			{
				if(!_prejfile && _info && _info->prejfile && HasLocalInfo)
					_prejfile = SvnBase::Utf8_PtrToString(_info->prejfile);

				return _prejfile;
			}
		}

		property SvnDepth Depth
		{
			SvnDepth get()
			{
				return _depth;
			}
		}

		property String^ ChangeList
		{
			String^ get()
			{
				if(!_changelist && _info && _info->changelist)
					_changelist = SvnBase::Utf8_PtrToString(_info->changelist);

				return _changelist;
			}
		}

		property __int64 WorkingCopySize
		{
			__int64 get()
			{
				return _wcSize;
			}
		}

		property __int64 RepositorySize
		{
			__int64 get()
			{
				return _size;
			}
		}

	public:
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if(keepProperties)
				{
					// Use all properties to get them cached in .Net memory
					GC::KeepAlive(Path);
					GC::KeepAlive(Uri);
					GC::KeepAlive(RepositoryRoot);
					GC::KeepAlive(RepositoryId);
					GC::KeepAlive(LastChangeAuthor);
					GC::KeepAlive(Lock);
					GC::KeepAlive(CopyFromUri);
					GC::KeepAlive(Checksum);
					GC::KeepAlive(ConflictOld);
					GC::KeepAlive(ConflictNew);
					GC::KeepAlive(ConflictWork);
					GC::KeepAlive(PropertyEditFile);
					GC::KeepAlive(ChangeList);
				}

				if(_lock)
					_lock->Detach(keepProperties);
			}
			finally
			{
				_info = nullptr;
				__super::Detach(keepProperties);
			}
		}
	};

	public ref class SvnErrorEventArgs : public SvnCancelEventArgs
	{
		initonly SvnException ^_exception;
	public:
		SvnErrorEventArgs(SvnException ^exception)
		{
			if(!exception)
				throw gcnew ArgumentNullException("exception");

			_exception = exception;
		}

		property SvnException^ Exception
		{
			SvnException^ get()
			{
				return _exception;
			}
		}
	};

	public ref class SvnDirEntry
	{
		const svn_dirent_t *_entry;
		initonly SvnNodeKind _nodeKind;
		initonly __int64 _fileSize;
		initonly bool _hasProperties;
		initonly __int64 _rev;
		initonly DateTime _time;
		String^ _author;
	internal:
		SvnDirEntry(const svn_dirent_t *entry)
		{
			if(!entry)
				throw gcnew ArgumentNullException("entry");

			_entry = entry;
			_nodeKind = (SvnNodeKind)entry->kind;
			_fileSize = entry->size;
			_hasProperties = (entry->has_props != 0);
			_rev = entry->created_rev;
			_time = (entry->time != 0) ? SvnBase::DateTimeFromAprTime(entry->time) : DateTime::MinValue;
		}
	public:
		property SvnNodeKind NodeKind
		{
			SvnNodeKind get()
			{
				return _nodeKind;
			}
		}

		/// <summary>Gets the length of the file text or 0 for directories</summary>
		property __int64 FileSize
		{
			__int64 get()
			{
				return _fileSize;
			}
		}

		/// <summary>Gets a boolean indicating whether this node has svn properties</summary>
		property bool HasProperties
		{
			bool get()
			{
				return _hasProperties;
			}
		}

		/// <summary>Gets the last revision in which this node changed</summary>
		property __int64 Revision
		{
			__int64 get()
			{
				return _rev;
			}
		}

		/// <summary>Gets the time of the last change</summary>
		property DateTime Time
		{
			DateTime get()
			{
				return _time;
			}
		}

		/// <summary>Gets the author of the last revision of this file</summary>
		property String^ Author
		{
			String^ get()
			{
				if(!_author && _entry->last_author)
					_author = SvnBase::Utf8_PtrToString(_entry->last_author);

				return _author;
			}
		}

	public:
		virtual void Detach(bool keepProperties)
		{
			try
			{
				if(keepProperties)
				{
					GC::KeepAlive(Author);
				}

			}
			finally
			{
				_entry = nullptr;

				//__super::Detach(keepProperties);
			}
		}
	};


	public ref class SvnListEventArgs : public SvnCancelEventArgs
	{
		initonly String^ _path;
		const char* _pAbsPath;
		const svn_lock_t *_pLock;
		const svn_dirent_t *_pDirEnt;

		String^ _absPath;
		SvnLockInfo^ _lock;
		SvnDirEntry^ _entry;

	internal:
		SvnListEventArgs(const char *path, const svn_dirent_t *dirent, const svn_lock_t *lock, const char *abs_path)
		{
			if(!path)
				throw gcnew ArgumentNullException("path");
			else if(!abs_path)
				throw gcnew ArgumentNullException("abs_path");

			_path = SvnBase::Utf8_PtrToString(path);

			_pDirEnt = dirent;
			_pLock = lock;
			_pAbsPath = abs_path;
		}

	public:
		/// <summary>Gets the path of the item</summary>
		property String^ Path
		{
			String^ get()
			{
				return _path;
			}
		}

		/// <summary>Gets the origin path of the item</summary>
		property String^ BasePath
		{
			String^ get()
			{
				if(!_absPath && _pAbsPath)
					_absPath = SvnBase::Utf8_PtrToString(_pAbsPath);

				return _absPath;
			}
		}

		property SvnLockInfo^ Lock
		{
			SvnLockInfo^ get()
			{
				if(!_lock && _pLock)
					_lock = gcnew SvnLockInfo(_pLock, false);

				return _lock;
			}
		}

		property SvnDirEntry^ Entry
		{
			SvnDirEntry^ get()
			{
				if(!_entry && _pDirEnt)
					_entry = gcnew SvnDirEntry(_pDirEnt);

				return _entry;
			}
		}


	public:
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if(keepProperties)
				{
					GC::KeepAlive(Path);
					GC::KeepAlive(BasePath);
					GC::KeepAlive(Lock);
					GC::KeepAlive(Entry);
				}

				if(_lock)
					_lock->Detach(keepProperties);
				if(_entry)
					_entry->Detach(keepProperties);
			}
			finally
			{
				_pAbsPath = nullptr;
				_pLock = nullptr;
				_pDirEnt = nullptr;

				__super::Detach(keepProperties);
			}
		}
	};

	public ref class SvnPropertyListEventArgs : public SvnCancelEventArgs
	{
		initonly String^ _path;
		apr_hash_t* _propHash;
		SvnPropertyCollection^ _properties;
		AprPool^ _pool;

	internal:
		SvnPropertyListEventArgs(const char *path, apr_hash_t* prop_hash, AprPool ^pool)
		{
			if(!path)
				throw gcnew ArgumentNullException("path");
			else if(!prop_hash)
				throw gcnew ArgumentNullException("prop_hash");
			else if(!pool)
				throw gcnew ArgumentNullException("pool");

			_path = SvnBase::Utf8_PtrToString(path);
			_propHash = prop_hash;
			_pool = pool;
		}

	public:
		property String^ Path
		{
			String^ get()
			{
				return _path;
			}
		}

		property SvnPropertyCollection^ Properties
		{
			SvnPropertyCollection^ get()
			{
				if(!_properties && _propHash && _pool)
					_properties = SvnBase::CreatePropertyDictionary(_propHash, _pool);

				return _properties;
			}
		}

	public:
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if(keepProperties)
				{
					GC::KeepAlive(Path);
					GC::KeepAlive(Properties);
					//					GC::KeepAlive(Entry);
				}
			}
			finally
			{
				_propHash = nullptr;
				_pool = nullptr;

				__super::Detach(keepProperties);
			}
		}
	};


	public ref class SvnListChangeListEventArgs : public SvnCancelEventArgs
	{
		initonly String^ _path;
		initonly String^ _changelist;

	internal:
		SvnListChangeListEventArgs(const char *path, const char *changelist)
		{
			if(!path)
				throw gcnew ArgumentNullException("path");

			_path = SvnBase::Utf8_PtrToString(path)->Replace('/', System::IO::Path::DirectorySeparatorChar);
			_changelist = SvnBase::Utf8_PtrToString(changelist);
		}

	public:
		property String^ Path
		{
			String^ get()
			{
				return _path;
			}
		}

		property String^ ChangeList
		{
			String^ get()
			{
				return _changelist;
			}
		}

	public:
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if(keepProperties)
				{
					GC::KeepAlive(Path);
				}
			}
			finally
			{
				__super::Detach(keepProperties);
			}
		}
	};

	public ref class SvnDiffSummaryEventArgs : public SvnCancelEventArgs
	{
		const svn_client_diff_summarize_t *_diffSummary;
		String^ _path;
		initonly bool _propertiesChanged;
		initonly SvnNodeKind _nodeKind;
		initonly SvnDiffKind _diffKind;
	internal:
		SvnDiffSummaryEventArgs(const svn_client_diff_summarize_t *diffSummary)
		{
			if(!diffSummary)
				throw gcnew ArgumentNullException("diffSummary");

			_diffSummary = diffSummary;
			_propertiesChanged = (0 != diffSummary->prop_changed);
			_nodeKind = (SvnNodeKind)diffSummary->node_kind;
			_diffKind = (SvnDiffKind)diffSummary->summarize_kind;
		}


	public:
		property String^ Path
		{
			String^ get()
			{
				if(!_path && _diffSummary)
					_path = SvnBase::Utf8_PtrToString(_diffSummary->path);

				return _path;
			}
		}

		property bool PropertiesChanged
		{
			bool get()
			{
				return _propertiesChanged;
			}
		}

		property SvnNodeKind NodeKind
		{
			SvnNodeKind get()
			{
				return _nodeKind;
			}
		}

		property SvnDiffKind DiffKind
		{
			SvnDiffKind get()
			{
				return _diffKind;
			}
		}

	public:
		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if(keepProperties)
				{
					GC::KeepAlive(Path);
				}
			}
			finally
			{
				_diffSummary = nullptr;
				__super::Detach(keepProperties);
			}
		}
	};

	public ref class SvnBlameEventArgs : public SvnCancelEventArgs
	{
		initonly __int64 _revision;
		initonly __int64 _lineNr;
		const char* _pcAuthor;
		const char* _pcLine;
		initonly DateTime _date;
		String^ _author;
		String^ _line;
		initonly __int64 _mergedRevision;
		const char* _pcMergedAuthor;
		const char* _pcMergedPath;
		initonly DateTime _mergedDate;
		String^ _mergedAuthor;
		String^ _mergedPath;

	internal:
		SvnBlameEventArgs(__int64 revision, __int64 lineNr, const char* author, const char* date, 
			svn_revnum_t merged_revision, const char *merged_author, const char *merged_date, 
			const char *merged_path, const char* line, AprPool^ pool)
		{
			if(!pool)
				throw gcnew ArgumentNullException("pool");
			else if(!author)
				throw gcnew ArgumentNullException("author");
			else if(!date)
				throw gcnew ArgumentNullException("date");
			else if(!line)
				throw gcnew ArgumentNullException("line");

			_revision = revision;
			_lineNr = lineNr;
			_pcAuthor = author;
			_pcLine = line;

			apr_time_t when = 0; // Documentation: date must be parsable by svn_time_from_cstring()
			svn_error_t *err = svn_time_from_cstring(&when, date, pool->Handle); // pool is not used at this time (might be for errors in future versions)

			if(!err)
				_date = SvnBase::DateTimeFromAprTime(when);

			_mergedRevision = merged_revision;
			_pcMergedAuthor = merged_author;
			_pcMergedPath = merged_path;

			if(merged_date)
			{
				err = svn_time_from_cstring(&when, merged_date, pool->Handle);

				if(!err)
					_mergedDate = SvnBase::DateTimeFromAprTime(when);
			}
		}

	public:
		property __int64 Revision
		{
			__int64 get()
			{
				return _revision;
			}
		}

		property __int64 LineNumber
		{
			__int64 get()
			{
				return _lineNr;
			}
		}

		property DateTime Date
		{
			DateTime get()
			{
				return _date; 
			}
		}

		property String^ Author
		{
			String^ get()
			{
				if(!_author && _pcAuthor)
					_author = SvnBase::Utf8_PtrToString(_pcAuthor);

				return _author;
			}
		}

		property String^ Line
		{
			String^ get()
			{
				if(!_line && _pcLine)
				{
					try
					{
						_line = SvnBase::Utf8_PtrToString(_pcLine);
					}
					catch(ArgumentException^)
					{
						_line = SharpSvnStrings::NonUtf8ConvertableLine;
					}
				}

				return _line;
			}
		}

		property String^ MergedAuthor
		{
			String^ get()
			{
				if(!_mergedAuthor && _pcMergedAuthor)
					_mergedAuthor = SvnBase::Utf8_PtrToString(_pcMergedAuthor);

				return _mergedAuthor;
			}
		}

		property String^ MergedPath
		{
			String^ get()
			{
				if(!_mergedPath && _pcMergedPath)
					_mergedPath = SvnBase::Utf8_PtrToString(_pcMergedPath);

				return _mergedPath;
			}
		}

		property DateTime MergedTime
		{
			DateTime get()
			{
				return _mergedDate; 
			}
		}

		property __int64 MergedRevision
		{
			__int64 get()
			{
				return _mergedRevision;
			}
		}

		virtual void Detach(bool keepProperties) override
		{
			try
			{
				if(keepProperties)
				{
					GC::KeepAlive(Author);
					GC::KeepAlive(Line);
					GC::KeepAlive(MergedAuthor);
					GC::KeepAlive(MergedPath);
				}
			}
			finally
			{
				_pcAuthor = nullptr;
				_pcLine = nullptr;
				_pcMergedAuthor = nullptr;
				_pcMergedPath = nullptr;
				__super::Detach(keepProperties);
			}
		}
	};
}
