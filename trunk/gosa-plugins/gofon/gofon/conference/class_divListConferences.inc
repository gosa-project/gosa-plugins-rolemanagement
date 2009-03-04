<?php

class divListConference extends MultiSelectWindow
{

  /* Current base */
  var $selectedBase       = "";
  var $departments        = array();

  /* Regex */
  var $Regex              = "*";

  /* checkboxes */
  var $ShowSendBocklists      ;
  var $ShowReceiveMacros  ;

  /* Subsearch checkbox */
  var $SubSearch;

  var $parent             ;
  var $ui                 ;

  function divListConference (&$config,$parent)
  {
    MultiSelectWindow::MultiSelectWindow($config,"Conferences", "gofonconference");
    
    $this->parent       = $parent;
    $this->ui           = get_userinfo();

    /* Set list strings */
    $this->SetTitle(_("List of conference rooms"));
    $this->SetSummary(_("List of conference rooms"));

    /* Result page will look like a headpage */
    $this->SetHeadpageMode();

    $this->EnableAplhabet(true);
  
    /* Disable buttonsm */
    $this->EnableCloseButton(false);
    $this->EnableSaveButton (false);

    /* Dynamic action col, depending on snapshot icons */
    $action_col_size = 100;
    if($this->parent->snapshotEnabled()){
      $action_col_size += 20;
    }

    /* Toggle all selected / deselected */
    $chk = "<input type='checkbox' id='select_all' name='select_all' title='"._("Select all")."'
      onClick='toggle_all_(\"^item_selected_[0-9]*$\",\"select_all\");' >";

    /* set Page header */
    $this->AddHeader(array("string"=> $chk,          "attach"=>"style='width:20px;'"));
    $this->AddHeader(array("string" => "&nbsp;", "attach" => "style='text-align:center;width:20px;'"));
    $this->AddHeader(array("string" =>_("Name")." - "._("Number"), "attach" => "style=''"));
    $this->AddHeader(array("string" => _("Owner"), "attach" => "style='width:200px;'"));
    $this->AddHeader(array("string" => _("PIN"), "attach" => "style='width:50px;'"));
    $this->AddHeader(array("string" =>_("Actions"), "attach" => "style='width:".$action_col_size."px;border-right:0px;text-align:right;'"));

    /* Add Checkboxes / SubSearch checkbox */
    $this->AddCheckBox("SubSearch",  msgPool::selectToView("","subsearch"), msgPool::selectToView("","subsearch_small"), false);

    /*                  Name                 ,Text                              ,Default  , Connect with alphabet  */
    $this->AddRegex   ("Regex",      _("Regular expression for matching conference names"),"*" , true);
  }


  function GenHeader()
  {
    /* Get all departments within this subtree */
    $base = $this->config->current['BASE'];
    $options  = $this->create_department_list($this->module);

    /* Get acls */
    $acls     = $this->ui->get_permissions($this->selectedBase,"gofonconference/conference");
    $acl_all  = $this->ui->has_complete_category_acls($this->selectedBase,"gofonconference") ;

    /* Add default header */
    $listhead = MultiSelectWindow::get_default_header();

    /* Add the rest (base select ....)*/
    $listhead .= _("Base")."&nbsp;<select name='CurrentMainBase' onChange='mainform.submit()' class='center'>$options</select>".
      " <input class='center' type='image' src='images/lists/submit.png' align='middle' 
      title='"._("Submit department")."' name='submit_department' alt='".  _("Submit").     "'>&nbsp;";

    /* Create Layers menu */
    $s  = ".|"._("Actions")."|\n";

    /* Append create options */
    if(preg_match("/(c.*w|w.*c)/",$acl_all)){
      $s .= "..|<img src='images/lists/new.png' alt='' border='0' class='center'>".
        "&nbsp;"._("Create")."|\n";
      $s.= "...|<input class='center' type='image' src='plugins/gofon/images/list_new_conference.png' alt=''>".
        "&nbsp;"._("Conference")."|conference_new|\n";
      $s.= "..|---|\n";
    }

    /* Multiple options */
    $s.= "..|<img src='images/lists/trash.png' alt='' border='0' class='center'>".
      "&nbsp;"._("Remove")."|"."remove_multiple|\n";

    /* Add Copy & Paste header */
    $s .= $this->parent->get_copypaste_header($this->selectedBase,$this->module);

    $s .= $this->parent->get_snapshot_header($this->selectedBase,$this->module);

    $this->SetDropDownHeaderMenu($s);
    $this->SetListHeader($listhead);
  }

  function execute()
  {
    $this->ClearElementsList();
    $this->GenHeader();
  }

  function setEntries($list)
  {
    $userimg  = "<img class='center' src='plugins/gofon/images/select_conference.png' alt='User' title='%s'>";
    $editlink = "<a href='?plug=".$_GET['plug']."&amp;id=%s&amp;act=edit_entry'>%s</a>";

    /* Dynamic action col, depending on snapshot icons */
    $action_col_size = 100;
    if($this->parent->snapshotEnabled()){
      $action_col_size += 20;
    }

    $ui = get_userinfo();

    /* Insert conferneces*/
    foreach($list as $conferencekey => $conference ){

      $acl      = $this->ui->get_permissions($conference['dn'],"gofonconference/conference");
      $acl_all  = $this->ui->has_complete_category_acls($conference['dn'],"gofonconference");

      /* You will need at least read access for the 
          current conference informations to display it */  
      if(!preg_match("/r/",$acl)){
        continue;
      }

      $actions ="";

      /* Add copy & cut functionality */
      $actions.= $this->parent->get_copypaste_action($conference['dn'],"gofonconference","conference");

      /* Add edit icon - This is allowed when we have at least read access. */
      $actions.= "<input class='center' type='image' src='images/lists/edit.png'     
                    alt='"._("edit")."'    name='conference_edit_%KEY%' title='"._("Edit this entry")."'>";

      /* Add snapshot icon */
      $actions.= $this->parent->get_snapshot_action($conference['dn'],$this->module);

      /* Create delete link - Only if we are allowed to delete this entry */
      if(preg_match("/d/",$acl)){
        $actions.= "<input class='center' type='image' src='images/lists/trash.png' 
          alt='"._("delete")."'   name='conference_del_%KEY%'  title='"._("Delete this entry")."'>";
      }

      /* Display PIN icon, only if we are the owner of the given conference */
      $owner = $this->parent->ui->dn==$conference['goFonConferenceOwner'][0];
      $p_acl = $this->ui->get_permissions($conference['dn'],"gofonconference/conference","goFonPIN");
      if((isset($conference['goFonPIN'][0])) && $owner && preg_match("/r/",$p_acl)){
        $pin = "<img class='center' src='plugins/gofon/images/pin.png' title='PIN : ".$conference['goFonPIN'][0]."' alt='PIN'>";
      }else{
        $pin = "&nbsp;";
      }

      /* Display numer of the conference if we are allowed to view it */
      $n_acl = $this->ui->get_permissions($conference['dn'],"gofonconference/conference","telephoneNumber");
      if(isset($conference['telephoneNumber'][0]) && preg_match("/r/",$n_acl)){
        $number = " - ".$conference['telephoneNumber'][0];
      }else{
        $number = "&nbsp; - ?";
      }

      /* Get conference owner name */
      $ldap= $this->parent->config->get_ldap_link();
      $ldap->cat($conference['goFonConferenceOwner'][0], array('cn','dn'));
      $data = $ldap->fetch();
      if(isset($data['cn'][0])){
        $cn = $data['cn'][0];
      }else{
        $cn = _("Unknown"); 
      }  

      /* Create title */
      $title="";
      if(isset($data['dn'])){
        $title = " title='".preg_replace("/ /","&nbsp;",LDAP::fix($data['dn']))."' ";
      }

      /* Cutted objects should be displayed in light grey */
      $display = $conference['cn'][0].$number;
      if($this->parent->CopyPasteHandler){
        foreach($this->parent->CopyPasteHandler->queue as $queue_key => $queue_data){
          if($queue_data['dn'] == $conference['dn']) {
            $display = "<font color='#999999'>".$display."</font>";
            break;
          }
        }
      }

      /* Create each field */
      $field0 = array("string" => "<input type='checkbox' id='item_selected_".$conferencekey."' name='item_selected_".$conferencekey."'>" ,
          "attach" => "style='width:20px;'");
      $a_field1 = array("string"=>sprintf($userimg,_("Conference")), "attach" => $title." style='text-align:center;width:20px;'");
      $a_field2 = array("string"=>sprintf($editlink,$conferencekey,$display), "attach" => $title." style=''");
      $a_field3 = array("string"=> $cn , "attach" =>  $title." style='width:200px;'");
      $a_field4 = array("string"=> $pin, "attach" =>  $title." style='width:50px;'");
      $a_field5 = array("string"=> preg_replace("/%KEY%/",$conferencekey,$actions), 
                        "attach"=> $title."style='width:".$action_col_size."px;border-right:0px;text-align:right;'");

      $this->AddElement(array($field0,$a_field1,$a_field2,$a_field3,$a_field4,$a_field5));
    }

    /* Create summary string for list footer */
    $num_deps=0;
    if(!$this->SubSearch){
      $num_deps = count($this->Added_Departments);
    }
    $num_objs = count($list);

    $num_obj_str = sprintf(_("Number of listed '%s'"),_("conferences"));
    $num_dep_str = sprintf(_("Number of listed '%s'"),_("departments"));

    $str = "<img class='center' src='plguins/gofon/images/select_conference.png'
              title='".$num_obj_str."' alt='".$num_obj_str."'>&nbsp;".$num_objs."&nbsp;&nbsp;&nbsp;&nbsp;";
    $str.= "<img class='center' src='images/lists/folder.png'
              title='".$num_dep_str."' alt='".$num_dep_str."'>&nbsp;".$num_deps."&nbsp;&nbsp;&nbsp;&nbsp;";

    $this->set_List_Bottom_Info($str);
  }

  function Save()
  {
    MultiSelectWindow :: Save();  
  }

  function save_object()
  {
    /* Save automatic created POSTs like regex, checkboxes */
    MultiSelectWindow :: save_object();   
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>