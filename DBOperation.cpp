#include<iostream>
#include<string>
#include<json/json.h>
#include<string.h>
#include<stdio.h>
#include"DBOperation.h"
#include<string.h>

ClientReq Parser::analyze(const Json::Value &root) {

	 ClientReq cli; 
	 int flg = 101;
         if(!root["reason_type"].empty())
	 {
		flg = root["reason_type"].asInt();
	 }
	 else
	 {
		cli.flag = 101;
	 }
	if(flg == 101)
		cli.flag = 101;
	 int hi_type = 0;
	 int topic_type = 0;
	 int hot_topic_type = 0;
	 if(!root["hi_type"].empty()){     //打招呼类型
		 hi_type= root["hi_type"].asInt(); 
	 }
	 if(!root["topic_type"].empty()){      //话题类型
		topic_type = root["topic_type"].asInt();                        
	 }
	 if(!root["hot_type"].empty()){
		hot_topic_type  = root["hot_type"].asInt();
	 }
     	 const char *corpus = root["corpus"].asString().c_str();    //语料，来获取该语料点赞/警告数
	 
	int _do = 101;
	if(root["fun"].empty())
	{
		cli.flag = 101;                                                   //操作
	}
	else
	{
		_do = root["fun"].asInt();
	}
	if(_do == 101)
	{
		cli.flag = 101;
	}
	int pageNo = ERR;
	if(!root["pageNo"].empty())
	{
		pageNo = root["pageNo"].asInt();
	}
	else
		cli.flag = 101;

	int pageSize = ERR;
	if(!root["pageSize"].empty())
	{
		pageSize = root["pageSize"].asInt();
	}
	else
		cli.flag = 101;
	if(pageNo == 101 || pageSize == 101)
		cli.flag = 101;
	 if(!root["user_send"].empty())
	 {
		_do_Reply(cli,root["user_send"].asString().c_str(),pageNo,pageSize);
		if(!root["hot"].empty())
			_do_HotNum(cli,5,corpus,0);
		if(!root["warn"].empty())
			_do_HotNum(cli,5,corpus,0);
	 }


	if(flg == 1)
	{
		_doHi_Table(cli,corpus,hi_type,_do,pageNo,pageSize); 
	}
	else if(flg == 2)
	{
		_doTopic_Table(cli,corpus,topic_type,_do,pageNo,pageSize);
	}
	else if(flg == 3)
	{
		_doCorpus_Table(cli,corpus,_do,pageNo,pageSize);  
	}
	else if(flg == 4)  //热门话题表
	{
		_doHotTopic_Table(cli,corpus,hot_topic_type,_do,pageNo,pageSize);
	}
	return cli;
}

void Parser::_do_Reply(ClientReq& cli,const char *corpus,int pageNo,int pageSize)
{
	if(corpus == NULL)
		cli.flag = 101;
	char *ptr = new char[256];
	memset(ptr,0,256);
	strncpy(ptr,corpus,strlen(corpus));

	cli.table_names.append("Reply");
	cli.cols.append("corpus");

	int start = (pageNo-0)*pageSize;
	char *sql = new char[256];
	memset(sql,0,256);
	snprintf(sql+strlen(sql),255,"%s","usersend like '%");
	snprintf(sql+strlen(sql),255,"%s",ptr);	
	snprintf(sql+strlen(sql),255,"%s","%' ");	
	snprintf(sql+strlen(sql),255,"%s","limit ");	
	snprintf(sql+strlen(sql),255,"%d",start);
	snprintf(sql+strlen(sql),255,"%s",",");	
	snprintf(sql+strlen(sql),255,"%d",pageSize);	
	
	cli.conditions.append(sql);
	delete []ptr;
	delete []sql;
}


void Parser::_doHi_Table(ClientReq& cli,const char *corpus,int hi_type,int _do,int pageNo,int pageSize)
{
		switch (_do)
		 {
		 case 0:
			 _do_Insert(cli,1,corpus,hi_type); break;
		 case 1:
			 _do_HotCorpus(cli,1,hi_type,pageNo,pageSize); break;
		 case 2:
			 _do_HotNum(cli,1,corpus,hi_type); break;
		 case 3:
			 _do_WarnNum(cli,1,corpus,hi_type); break;
		 default:
			cli.flag = 101; break;
		 }
}

void Parser::_doTopic_Table(ClientReq &cli,const char *corpus,int topic_type,int _do,int pageNo,int pageSize)
{
	switch (_do)
	{
	case 0:
		_do_Insert(cli,2,corpus,topic_type); break;
	case 1:
		_do_HotCorpus(cli,2,topic_type,pageNo,pageSize); break;
	case 2:
		_do_HotNum(cli,2,corpus,topic_type); break;
	case 3:
		_do_WarnNum(cli,2,corpus,topic_type); break;
	default:
		cli.flag = 101; break;
	}
}

void Parser::_doCorpus_Table(ClientReq& cli,const char *corpus,int _do,int pageNo,int pageSize)
{
	switch (_do)
	{
	case 0:
		_do_Insert(cli,3,corpus,0); break;
	case 1:
		_do_HotCorpus(cli,3,0,pageNo,pageSize); break;
	case 2:
		_do_HotNum(cli,3,corpus,0); break;
	case 3:
		_do_WarnNum(cli,3,corpus,0); break;
	default:
		cli.flag = 101; break;
	}
}

void Parser::_doHotTopic_Table(ClientReq &cli,const char *corpus,int hot_topic_type,int _do,int pageNo,int pageSize)
{
	switch (_do)
	{
	case 0:
		_do_Insert(cli,4,corpus,hot_topic_type); break;
	case 1:
		_do_HotCorpus(cli,4,hot_topic_type,pageNo,pageSize); break;
	case 2:
		_do_HotNum(cli,4,corpus,hot_topic_type); break;
	case 3:
		_do_WarnNum(cli,4,corpus,hot_topic_type); break;
	default:
		cli.flag = 101; break;
	}
}

void Parser::_do_Insert(ClientReq& cli,int table,const char *corpus,int type)/*table名，语料，语料类型*/
{	
	cli.flag = 1;
        
	char *ptr = new char[256];
	memset(ptr,0,256);
	strncpy(ptr,corpus,strlen(corpus));
	if(table == 1)  //贡献开场白
	{
		cli.table_names.append("sayhi_table");
		cli.cols.append("(corpus,hitype,hot,warn)");
	}
	else if(table == 2)   //贡献话题
	{
		cli.table_names.append("topic_table");
		cli.cols.append("(corpus,topictype,hot,warn)");
	}
	else if(table == 3)   //贡献骚话
	{
		cli.table_names.append("corpus_table");
		cli.cols.append("(corpus,corpus_type,hot,warn)");
	}
	else if(table == 4)
	{
		cli.table_names.append("hottopic_table");
		cli.cols.append("(corpus,hottopictype,hot,warn)");
	}
	else
		cli.flag = 101;
	/*
	*value()中的值
	*/
	char *sql =new char[256];
        memset(sql,0,256);
	snprintf(sql+strlen(sql),255,"%s","('");
	snprintf(sql+strlen(sql),255,"%s",ptr);
	snprintf(sql+strlen(sql),255,"%s","',");
	snprintf(sql+strlen(sql),255,"%d",type);
	snprintf(sql+strlen(sql),255,"%s",",0,0)");

	cli.conditions.append(sql);
	
	delete []ptr;
	delete []sql;
}

void Parser::_do_HotCorpus(ClientReq &cli,int table,int type,int pageNo,int pageSize)
{
	cli.flag=2;
	char *sql = new char[256];
        memset(sql,0,256);
	if (table==1)
	{
		cli.table_names.append("sayhi_table");
		cli.cols.append("corpus,hot");	
		strncpy(sql,"hitype=",strlen("hitype="));
	}
	else if (table==2)
	{
		cli.table_names.append("topic_table");
		cli.cols.append("corpus,hot");
		strncpy(sql,"topictype=",strlen("topictype="));
	}
	else if (table==3)
	{
		cli.table_names.append("corpus_table");
		cli.cols.append("corpus, hot");
		strncpy(sql,"corpustype=",strlen("courpstype="));
	}
	else if (table==4)
	{
		cli.table_names.append("hottopic_table");
		cli.cols.append("corpus, hot");
		strncpy(sql,"hottopictype=",strlen("hottopictype="));
	}
	else
		cli.flag = 101;

	int start = (pageNo-0)*pageSize;
	snprintf(sql+strlen(sql),255,"%d",type);
	snprintf(sql+strlen(sql),255,"%s"," order by hot desc limit ");
	snprintf(sql+strlen(sql),255,"%d",start);
	snprintf(sql+strlen(sql),255,"%s",",");
	snprintf(sql+strlen(sql),255,"%d",pageSize);

	
	cli.conditions.append(sql);		

	delete []sql;
}

void Parser::_do_HotNum(ClientReq& cli,int table,const char *corpus,int type)
{
	cli.flag=3;
	
	char *ptr = new char[256];
	memset(ptr,0,256);
	strncpy(ptr,corpus,strlen(corpus));
	
	if (table==1)
	{
		cli.table_names.append("sayhi_table");
		cli.cols.append("hot=hot+1");
	}
	else if (table==2)
	{
		cli.table_names.append("topic_table");
		cli.cols.append("hot=hot+1"); 
	}
	else if (table==3)
	{
		cli.table_names.append("corpus_table");
		cli.cols.append("hot=hot+1");
	}
	else if (table==4)
	{
		cli.table_names.append("hottopic_table");
		cli.cols.append("hot=hot+1");
	}
	else if(table == 5)
	{
		cli.table_names.append("Reply");
		cli.cols.append("hot=hot+1");	
	}
	else
		cli.flag = 101;
	
	char *sql=new char[256];
        memset(sql,0,256);
	snprintf(sql+strlen(sql),255,"%s","corpus='");
	snprintf(sql+strlen(sql),255,"%s",ptr);
	snprintf(sql+strlen(sql),255, "%s", "'");
	cli.conditions.append(sql);

	delete []ptr;
	delete []sql;
}

void Parser::_do_WarnNum(ClientReq &cli,int table,const char *corpus,int type)
{
	cli.flag = 3;

	char *ptr = new char[256];
	memset(ptr,0,256);
	strncpy(ptr,corpus,strlen(corpus));
	if(table == 1)  //开场白警告
	{
		cli.table_names.append("sayhi_table");
		cli.cols.append("warn=warn+1");
	}
	else if(table == 2)   //话题语料警告
	{
		cli.table_names.append("topic_table");
		cli.cols.append("warn=warn+1");
	}
	else if(table == 3)   //骚话警告
	{
		cli.table_names.append("corpus_table");
		cli.cols.append("warn=warn+1");
	}
	else if(table == 4)
	{
		cli.table_names.append("hottopic_table");
		cli.cols.append("warn=warn+1");
	}
	else if(table == 5)
	{
		cli.table_names.append("Reply");
		cli.cols.append("warn=warn+1");
	}
	else
		cli.flag = 101;
	/*
	*value()中的值
	*/
	char *sql = new char[256];
        memset(sql,0,256);
	snprintf(sql+strlen(sql),255,"%s","corpus='");
	snprintf(sql+strlen(sql),255,"%s",ptr);
	snprintf(sql+strlen(sql),255,"%s","'");
			 
	cli.conditions.append(sql);

	delete []ptr;
	delete []sql;
}


  
   


