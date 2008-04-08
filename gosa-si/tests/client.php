#!/usr/bin/php5 -q
<?php

require_once("../../gosa-core/include/class_socketClient.inc");
error_reporting(E_ALL);

$zahl= 1;

for($count = 1; $count <= $zahl; $count++)
{

$sock = new Socket_Client("127.0.0.1","20081",TRUE,1);
$sock->setEncryptionKey("secret-gosa-password");

if($sock->connected()){
	/* Prepare a hunge bunch of data to be send */

# jobdb add
#$data = "<xml> <header>gosa_network_completition</header> <source>GOSA</source><target>GOSA</target><hostname>ws-muc-2</hostname></xml>";
#$data = "<xml> <header>job_sayHello</header> <source>10.89.1.155:20083</source><target>00:01:6c:9d:b9:fa</target><mac>00:1B:77:04:8A:6C</mac> <timestamp>20130102133900</timestamp> </xml>";
#$data = "<xml> <header>job_ping</header> <source>GOSA</source> <target>00:01:6c:9d:b9:fa</target> <macaddress>00:01:6c:9d:b9:fa</macaddress><timestamp>19700101000000</timestamp> </xml>";


# jobdb delete
#$data = "<xml> <header>gosa_delete_jobdb_entry</header> <source>GOSA</source> <target>GOSA</target> <where><clause><phrase><id>1</id></phrase></clause></where></xml>";

# smbhash
#$data = "<xml> <header>gosa_gen_smb_hash</header> <source>GOSA</source><target>GOSA</target><password>tester</password></xml>";

# Reload ldap config
#$data = "<xml> <header>gosa_trigger_reload_ldap_config</header> <source>GOSA</source><target>00:01:6c:9d:b9:fa</target></xml>";

# jobdb update  
#$data = "<xml> <header>gosa_update_status_jobdb_entry</header> <source>GOSA</source> <target>GOSA</target> <where><clause><phrase> <id>1</id></phrase></clause></where> <update><timestamp>19700101000000</timestamp></update></xml>";
#$data = "<xml> <header>gosa_update_status_jobdb_entry</header> <source>GOSA</source><target>GOSA</target><where><clause><phrase> <macaddress>00:01:6c:9d:b9:fa</macaddress></phrase></clause> </where> <update><status>processing</status> <result>update</result></update></xml>";

# jobdb query
#$data = "<xml><header>gosa_query_jobdb</header><source>GOSA</source> <target>GOSA</target><where><clause><connector>and</connector><phrase><operator>gt</operator><ROWID>0</ROWID></phrase><phrase><operator>le</operator><ROWID>5</ROWID></phrase></clause></where></xml>";
#$data = "<xml><header>gosa_query_jobdb</header><source>GOSA</source> <target>GOSA</target> <where> <clause> <phrase> <operator>like</operator> <timestamp>%0102%</timestamp> </phrase> </clause> </where> </xml>";
#$data= "<xml><header>gosa_query_jobdb</header><source>GOSA</source> <target>GOSA</target><where><clause><phrase><headertag>ping</headertag></phrase></clause></where><limit><from>0</from><to>3</to></limit></xml>";
#$data= "<xml><header>gosa_query_jobdb</header><source>GOSA</source> <target>GOSA</target><where><clause><phrase><HEADERTAG>trigger_action_reinstall</HEADERTAG></phrase></clause></where><limit><from>0</from><to>25</to></limit><orderby>timestamp DESC</orderby></xml>";
#$data= "<xml><header>gosa_query_jobdb</header><source>GOSA</source> <target>GOSA</target></xml>";

# jobdb count
#$data = "<xml> <header>gosa_count_jobdb</header><source>GOSA</source> <target>GOSA</target></xml>";
#$data = "<xml> <header>gosa_count_fai_server</header><source>GOSA</source> <target>GOSA</target></xml>";
#$data = "<xml> <header>gosa_count_fai_release</header><source>GOSA</source> <target>GOSA</target></xml>";

# jobdb clear
#$data = "<xml> <header>gosa_clear_jobdb</header> <source>GOSA</source> <target>GOSA</target></xml>";

# set gosa-si-client to 'activated'
#$data = "<xml> <header>job_set_activated_for_installation</header> <target>10.89.1.31:20083</target> <source>GOSA</source> <macaddress>00:01:6c:9d:b9:fa</macaddress><timestamp>22220101000000</timestamp></xml>";



# trigger jobs at client
#$data = "<xml> <header>gosa_detect_hardware</header> <target>10.89.1.31:20083</target> <source>10.89.1.31:20081</source> </xml>";
#$data = "<xml> <header>gosa_new_key_for_client</header> <target>00:01:6c:9d:b9:fa</target> <source>10.89.1.31:20081</source> </xml>";
#$data = "<xml> <header>job_trigger_action_wake</header> <target>00:01:6c:9d:b9:fa</target> <source>GOSA</source> <timestamp>19700101000000</timestamp></xml>";
#$data = "<xml> <header>gosa_trigger_action_faireboot</header> <target>00:01:6c:9d:b9:fa</target> <source>GOSA</source> </xml>";
#$data = "<xml> <header>job_trigger_action_reinstall</header> <source>GOSA</source> <target>00:01:6c:9d:b9:fa</target> <macaddress>00:01:6c:9d:b9:fa</macaddress> <timestamp>19700101000000</timestamp> </xml>";
#$data = "<xml> <header>job_trigger_action_instant_update</header> <target>00:01:6c:9d:b9:fa</target> <source>GOSA</source> <timestamp>19700101000000</timestamp> </xml>";
#$data = "<xml> <header>gosa_ping</header> <target>00:01:6c:9d:b9:fa</target> <source>GOSA</source> </xml>";


# get_login_usr_for_client
#$data = "<xml> <header>gosa_get_login_usr_for_client</header> <target>GOSA</target> <source>GOSA</source> <client>00:01:6c:9d:b9:fa</client></xml>";

# get_client_for_login_usr
#$data = "<xml> <header>gosa_get_client_for_login_usr</header> <target>GOSA</target> <source>GOSA</source> <usr>harald</usr></xml>";

# recreate_fai_server_db
#$data = "<xml> <header>gosa_recreate_fai_server_db</header> <target>GOSA</target> <source>GOSA</source></xml>"; 

# testing
#$data = "<xml> <header>gosa_send_user_msg</header> <target>GOSA</target> <source>GOSA</source> <user>susi</user> <user>harald</user> <user>susi</user> <group>gosa-admins-all</group> </xml>"; 
#$data = "<xml> <header>gosa_send_user_msg</header> <target>GOSA</target> <source>GOSA</source> <group>gosa-admins-all</group> <usrmesg>kaffeepause</usrmesg> </xml>"; 
#$data = "<xml> <header>gosa_send_user_msg</header> <target>GOSA</target> <source>GOSA</source> <user>cajus.pollmeier</user> <message>kaffeepause</message> </xml>"; 


################
# logHandling.pm
# all date and mac parameter accept regular expression as input unless other instructions are given
#$data = "<xml> <header>gosa_show_log_by_mac</header> <target>GOSA</target> <source>GOSA</source> <mac>00:01:6C:9D:B9:FA</mac> <mac>00:01:6c:9d:b9:fb</mac> </xml>"; 
#$data = "<xml> <header>gosa_show_log_by_date</header> <target>GOSA</target> <source>GOSA</source> <date>20080313</date> <date>20080323</date> </xml>"; 
#$data = "<xml> <header>gosa_show_log_by_date_and_mac</header> <target>GOSA</target> <source>GOSA</source> <date>200803</date> <mac>00:01:6c:9d:b9:FA</mac> </xml>"; 
# exact date and mac are required as input for show_log_files_by_date_and_mac
#$data = "<xml> <header>gosa_show_log_files_by_date_and_mac</header> <target>GOSA</target> <source>GOSA</source> <date>install_20080311_090900</date> <mac>00:01:6c:9d:b9:fa</mac> </xml>"; 
# exact date and mac parameter are required as input for get_log_file_by_date_and_mac
#$data = "<xml> <header>gosa_get_log_file_by_date_and_mac</header> <target>GOSA</target> <source>GOSA</source> <date>install_20080311_090900</date> <mac>00:01:6c:9d:b9:fa</mac> <log_file>boot.log</log_file></xml>"; 
#$data = "<xml> <header>gosa_delete_log_by_date_and_mac</header> <target>GOSA</target> <source>GOSA</source>  <mac>00:01:6c:9d:b9:fa</mac></xml>"; 
#  exact mac parameter are required as input for get_recent_log_by_mac
$data = "<xml> <header>gosa_get_recent_log_by_mac</header> <target>GOSA</target> <source>GOSA</source> <mac>00:01:6c:9d:b9:fA</mac></xml>"; 



    $sock->write($data);
    $answer = "nothing";
	$answer = $sock->read();
    
    echo "$count: $answer\n";
	$sock->close();	
}else{
	echo "... FAILED!\n";
}
}
?>
