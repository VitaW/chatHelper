#include<iostream>
#include<string>
#include<json/json.h>
#include<string.h>
#include<stdio.h>

#define ERR 101

#define INSERTION 1
#define SELECTION 2
#define UPDATION 3
#define DELETION 4
typedef struct ClientReq{
	int flag;                        //insert->1��select->2��update->3��delect->4
	std::string table_names;   
	std::string cols;          
	std::string conditions;    
}ClientReq;

class Parser{

public:
	static void _do_WarnNum(ClientReq& cli,int table,const char *corpus,int type);
	static void _do_HotNum(ClientReq &cli,int table,const char *corpus,int type);
	static void _do_HotCorpus(ClientReq& cli,int table,int type,int pageNo,int pageSize);
	static void _do_Insert(ClientReq& cli,int table,const char *corpus,int type);

	static void _do_Reply(ClientReq& cli,const char *corpus,int No,int size);	

	static void _doHi_Table(ClientReq& cli,const char *corpus,int hi_type,int _do,int No,int size);
	static void _doTopic_Table(ClientReq& cli,const char *corpus,int topic_type,int _do,int No,int size);
	static void _doCorpus_Table(ClientReq& cli,const char *corpus,int _do,int No,int size);
	static void _doHotTopic_Table(ClientReq& cli,const char *corpus,int hot_topic_type,int _do,int No,int size);

	static ClientReq analyze(const Json::Value &root);
	
};

