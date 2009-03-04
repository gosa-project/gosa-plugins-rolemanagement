<?php

class servrepository extends goService
{
  /* attribute list for save action */
  //  var $ignore_account         = TRUE;
  var $attributes             = array("FAIrepository");
  var $objectclasses          = array("FAIrepositoryServer");
  
  /* Repositories */
  var $repositories          = array();
  var $FAIrepository         = array();
  var $conflicts             = array("goKioskService");
  var $DisplayName           = "";
  var $StatusFlag            = "";
  
  var $view_logged            = FALSE;
  var $fai_activated          = FALSE;

  var $divlist = NULL;

  function servrepository (&$config, $dn= NULL, $parent= NULL)
  {
    plugin::plugin ($config, $dn,$parent);

    $this->DisplayName = _("Repository service");

    /* Skip this if fai is deactivated */
    $tmp= $this->config->search("faiManagement", "CLASS",array('menu','tabs'));
    if(!empty($tmp)){
      $this->fai_activated = TRUE;    
    }else{
      return;
    }

    $this->repositories = array();
    if(isset($this->attrs['FAIrepository'])){
      for($i = 0; $i < $this->attrs['FAIrepository']['count']; $i++){
        $tmp = split("\|",$this->attrs['FAIrepository'][$i]);
        $tmp2 = array();  
        $tmp3 = array();   

        if(isset($tmp[1])){
          $tmp2['ParentServer'] = $tmp[1];
          if(empty($tmp[1])){
            $tmp2['ParentServer'] = "none";
          }
        }else{
          $tmp2['ParentServer'] = "none";
        }

        if(isset($tmp[0])){
          $tmp2['Url']          = $tmp[0];
        }else{
          $tmp2['Url']          = "";
        }
  
        if(isset($tmp[2])){
          $tmp2['Release']      = $tmp[2];
        }else{
          $tmp2['Release']      = "";
        }

        if(isset($tmp[3])){
          $tmp3 = split(",",$tmp[3]);
          foreach($tmp3 as $sec){
            $tmp2['Sections'][$sec]=$sec;
          }    
        }else{
          $tmp['Section']=array();
        }

        $this->repositories[$tmp[2]]=$tmp2;      
      }
    }


    /* Create divlist */
    $this->divlist = new divListRepository($this->config,$this);
  }

  function execute()
  {
    /* Call parent execute */
    plugin::execute();

    if($this->is_account && !$this->view_logged){
      $this->view_logged = TRUE;
      new log("view","server/".get_class($this),$this->dn);
    }

    if(!$this->fai_activated){
      $str = "<h2>"._("You can't use this plugin until FAI is activated.")."</h2>";
      return $str;
    }

    /* Fill templating stuff */
    $smarty= get_smarty();
    $smarty->assign("is_createable",$this->acl_is_createable());
    $display= "";

    /* Show tab dialog headers */
    /*
       ADD / EDIT Repository
       Dialog Handling
     */
    $once = false;
    if(isset($_POST['servRepository'])){
      foreach($_POST as $name => $value){

        if(preg_match("/AddRepository/",$name) && $this->acl_is_createable()){
          $once = true;
          $this->dialog = new servRepositorySetup($this->config,$this->dn);
          $this->dialog->parent = $this;
        }

        if((preg_match("/^delete_/",$name)) && (!$once) && $this->acl_is_removeable()){
          $once = true;
          $value = preg_replace("/delete_/","",$name);
          $value = base64_decode(preg_replace("/_.*$/","",$value));

          $url = $this->repositories[$value]['Url'];
          $release = $this->repositories[$value]['Release'];

          $ldap = $this->config->get_ldap_link();
          $ldap->cd ($this->config->current['BASE']);

          $ldap->search("(&(objectClass=gotoWorkstation)(objectClass=FAIobject)(FAIdebianMirror=".$url."))",array("cn","FAIclass"));
          if ($ldap->count() != 0){
            $obj= array();
            $found= false;
            while($attrs = $ldap->fetch()){
              foreach($attrs['FAIclass'] as $class){
                if(preg_match("/".str_replace("/","\/",$release)."$/i",$class)){
                  $obj[$ldap->getDN()]= $attrs['cn'][0];
                  $found= true;
                }
              }
            }

            if ($found){
              msg_dialog::display(_("Error"), msgPool::stillInUse(_("FAI release"), msgPool::buildList($obj)), ERROR_DIALOG);
            }
          }else{
            if(isset($this->repositories[$value])){
              unset($this->repositories[$value]);
            }
          }
        }

        if((preg_match("/^edit_/",$name))&&(!$once)){
          $value = preg_replace("/edit_/","",$name);
          $value = base64_decode(preg_replace("/_.$/","",$value));

          if(isset($this->repositories[$value])){
            $once = true;
            $obj = $this->repositories[$value];

            /* to be able to detect if this was renamed */
            $obj['initialy_was'] = $obj['Release'];
            $this->dialog = new servRepositorySetup($this->config,$this->dn,$obj);
            $this->dialog->parent = $this;
          }
        }
      }
    }
    if((isset($_GET['act']))&&($_GET['act']=="open_repository")&&(isset($_GET['id']))){
      $obj = $this->repositories[base64_decode($_GET['id'])];
      $obj['initialy_was'] = $obj['Release'];
      $this->dialog = new servRepositorySetup($this->config,$this->dn,$obj);
      $this->dialog->parent = $this;
    }

    if(isset($_POST['repository_setup_save']) && is_object($this->dialog)){
      $this->dialog->save_object();
      if(($this->dialog->is_new_name())&&(isset($this->repositories[$this->dialog->GetName()]))){
        msg_dialog::display(_("Error"), msgPool::duplicated(_("Name")), ERROR_DIALOG);
      }else

      if(count($this->dialog->check())!=0){
        foreach($this->dialog->check() as $msg){
          msg_dialog::display(_("Error"), $msg, ERROR_DIALOG);
        }
      }else{
        $obj = $this->dialog->save();
        if($this->dialog->is_new_name()){
          $oldname = $this->dialog->initialy_was;
          $this->repositories[$obj['Release']]=$obj;        
          unset($this->repositories[$oldname]);
        }else{ 
          $this->repositories[$obj['Release']]=$obj;        
        }
        $this->dialog = FALSE;
        $this->is_dialog= false;
      }
    }

    if(isset($_POST['repository_setup_cancel'])){
      $this->dialog=FALSE;
      $this->is_dialog = false;
    }
   
    if(is_object($this->dialog)){
      $this->dialog->save_object();
      $this->is_dialog = true;
      return($this->dialog->execute());
    }

    /*
      Repository setup dialog handling /END
    */


    $link   = "<a href='?plug=".$_GET['plug']."&amp;act=open_repository&amp;id=%s'>%s</a>";
    $edit   = "<input type='image' value='%s' name='edit_%s'   src='images/lists/edit.png'>&nbsp;";
   
    /* Hide delete icon, if delete is not allowed */ 
    if($this->acl_is_removeable()){
      $delete = "<input type='image' value='%s' name='delete_%s' src='images/lists/trash.png'>";
    }else{
      $delete = "<img src='images/empty.png' alt='&nbsp;'>";
    }

    $this->divlist->execute(); 
    $this->divlist->setEntries($this->repositories);
    $smarty->assign("Repositories",$this->divlist->Draw());
    $display.= $smarty->fetch(get_template_path('servRepository.tpl', TRUE,dirname(__FILE__)));
    return($display);
  }


  /* Save data to object */
  function save_object()
  {
    plugin::save_object();
    if(is_object($this->divlist)){
      $this->divlist->save_object();
    }
  }


  /* Check supplied data */
  function check()
  {
    /* Call common method to give check the hook */
    $message= plugin::check();
    return ($message);
  }


  /* Save to LDAP */
  function save()
  {
    if(!$this->fai_activated) return;

    plugin::save();

    $arr = array();
    foreach($this->repositories as $servername => $conf){
      $str = "";
      foreach($conf['Sections'] as $sec){
        $str.=$sec.",";
      }
      $str=preg_replace("/,$/","",$str);
        
      if($conf['ParentServer']=="none"){
        $conf['ParentServer'] ="";
      }    
    
      $arr[]=$conf['Url']."|".$conf['ParentServer']."|".$conf['Release']."|".$str;
    }
    $this->attrs['FAIrepository'] = $arr;

    $ldap= $this->config->get_ldap_link();
    $ldap->cd ($this->config->current['BASE']);
    
    $ldap->cat($this->dn, array('dn'));
    
    if($ldap->count()){
      $ldap->cd($this->dn);
      $this->cleanup();
      $ldap->modify ($this->attrs);       
      $this->handle_post_events("modify");
    }else{
      $ldap->cd ($this->config->current['BASE']);
      $ldap->create_missing_trees($this->dn);
      $ldap->cd($this->dn);
      $ldap->add($this->attrs);
      $this->handle_post_events("add");
    }

    # If there were changes, just tell the server to reload information
    if(count($this->attrs)){
	    $this->trigger_si_fai_server_reload();
    }

    if($this->initially_was_account){
      new log("modify","server/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }else{
      new log("create","server/".get_class($this),$this->dn,array_keys($this->attrs),$ldap->get_error());
    }
  }


  function getListEntry()
  {
    $fields = goService::getListEntry();
    $fields['Message']    = _("Repository service");
    return($fields);
  }


  function trigger_si_fai_server_reload()
  {
    /* Reload GOsa si FAI DB/cache
     */
    if(class_available("DaemonEvent") && class_available("gosaSupportDaemon")){
      $events = DaemonEvent::get_event_types(SYSTEM_EVENT | HIDDEN_EVENT);        
      if(isset($events['TRIGGERED']['DaemonEvent_recreate_fai_server_db'])){
        $evt = $events['TRIGGERED']['DaemonEvent_recreate_fai_server_db']; 
        $tmp = new $evt['CLASS_NAME']($this->config);
        $tmp->set_type(TRIGGERED_EVENT);
        $tmp->add_targets(array("GOSA"));
        $o_queue = new gosaSupportDaemon();
        if(!$o_queue->append($tmp)){
          msg_dialog::display(_("Service infrastructure"),msgPool::siError($o_queue->get_error()),ERROR_DIALOG);
        }
      }
    }
  }


  function remove_from_parent()
  {
    goService::remove_from_parent();
    $this->trigger_si_fai_server_reload();
  }

  /* Return plugin informations for acl handling */
  static function plInfo()
  {
    return (array(
          "plShortName"   => _("Repository"),
          "plDescription" => _("Repository service")." ("._("Services").")",
          "plSelfModify"  => FALSE,
          "plDepends"     => array(),
          "plPriority"    => 84,
          "plSection"     => array("administration"),
          "plCategory"    => array("server"),

          "plProvidedAcls"=> array(
#            "start"         => _("Service start"),  // Remove this to hide the start button at all.
#            "stop"          => _("Service stop"),   // Remove this to hide the stop button at all.
#            "restart"       => _("Service restart"),// Remove this to hide the restart button at all.

#            "cn"            => _("Name"),
            "ParentServer"  => _("Parent server"),
            "Release"       => _("Releases"),
            "Url"           => _("URL"),
            "Section"       => _("Sections"))
          ));
  }
}

// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>