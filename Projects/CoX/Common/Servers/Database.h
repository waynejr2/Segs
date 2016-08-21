/*
 * Super Entity Game Server
 * http://segs.sf.net/
 * Copyright (c) 2006 - 2016 Super Entity Game Server Team (see Authors.txt)
 * This software is licensed! (See License.txt for details)
 *
 */
#pragma once

#include "DatabaseInterfaces.h"

#include <string>
#include <cassert>
#include <vector>
#include <string>
#include <stdint.h>
#include <QtCore/QDateTime>

using std::vector;
using std::string;

class Database;
class DbResultRow
{
protected:
        IResultRow *    m_impl;
public:
                        DbResultRow(IResultRow *a) : m_impl(a) {}
                        DbResultRow(DbResultRow &&other) : m_impl(other.m_impl) { other.m_impl=NULL; }
                        ~DbResultRow() { delete m_impl; }
        DbResultRow &   operator=(DbResultRow && from) {
                            delete m_impl;
                            m_impl = NULL;
                            std::swap(m_impl,from.m_impl);
                            return *this;
                        }
        bool            valid() const                         { return m_impl!=NULL;}
        const char *    getColString(const char *column_name) { return m_impl->getColString(column_name);}
        int16_t         getColInt16(const char *column_name)  { return m_impl->getColInt16(column_name);}
        int32_t         getColInt32(const char *column_name)  { return m_impl->getColInt32(column_name);}
        int64_t         getColInt64(const char *column_name)  { return m_impl->getColInt64(column_name);}
        int64_t         getColInt64(int col)                  { return m_impl->getColInt64(col);}
        bool            getColBool(const char *column_name)   { return m_impl->getColBool(column_name);}
        float           getColFloat(const char *column_name)  { return m_impl->getColFloat(column_name);}
        QDateTime       getTimestamp(const char *column_name) { return m_impl->getTimestamp(column_name);}
        IResultRow::vBinData    getColBinary(const char *column_name) { return m_impl->getColBinary(column_name); }

};

class DbResults
{
public:
        IResult *   m_impl;
        DbResults() : m_impl(NULL) {}
        ~DbResults() { delete m_impl; }
        const char *message() {return m_impl->message();}
        DbResultRow nextRow() { return DbResultRow(m_impl->nextRow());}
};

class Database
{
public:
virtual                 ~Database() {}
virtual void            setConnectionConfiguration(const char *host,const char *port,const char *db,const char *user,const char *passw)=0;
virtual bool            execQuery(const string &q,DbResults &res)=0;
virtual bool            execQuery(const string &q)=0; // for queries without results
virtual int             OpenConnection(IDataBaseCallbacks *)=0;
virtual int             CloseConnection(void)=0;
virtual int64_t         next_id(const std::string &tab_name)=0;
virtual IPreparedQuery *prepare(const std::string &query,size_t num_params)=0;
virtual IPreparedQuery *prepareInsert(const std::string &query,size_t num_params)=0;
virtual void *          conn() = 0; //!< get underlying db connection object

};
struct PreparedArgs
{
        typedef std::vector<std::string> vParams;

        vParams             m_params;
        std::vector<int>    m_lengths;
        std::vector<int>    m_formats;
        void                add_param(const uint8_t *bytes, size_t len, bool binary);
        void                add_param(const std::string &str);
        void                add_param(uint16_t);
        void                add_param(uint32_t);
        void                add_param(uint64_t);
};
class DbTransactionGuard
{
    Database &m_db;
    bool proper_commit;
public:
    DbTransactionGuard(Database &db) : m_db(db),proper_commit(false)
    {
        m_db.execQuery("BEGIN");
    }
    void commit() {proper_commit=true;}
    ~DbTransactionGuard()
    {
        if(proper_commit)
            m_db.execQuery("COMMIT;");
        else
            m_db.execQuery("ROLLBACK;");
    }

};
