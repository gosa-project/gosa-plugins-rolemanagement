<?php

class faiPartition extends plugin
{
  var $attributes = array("cn","description","FAIfsCreateOptions","FAIfsOptions","FAIfsTuneOptions",
      "FAIfsType","FAImountOptions","FAImountPoint","FAIpartitionNr","FAIpartitionFlags","FAIdiskType",
      "FAIpartitionSize","FAIpartitionType","FAIstate", "bootable","resize", "preserve", "preserveType",
      "encrypted");

  var $cn  = "";
  var $description  = "";
  var $FAIfsCreateOptions = "";
  var $FAIfsOptions = "";
  var $FAIfsTuneOptions = "";
  var $FAIfsType = "";
  var $FAImountOptions = "";
  var $FAImountPoint = "";
  var $FAIpartitionNr = "undefined"; // Initial value for new partitions
  var $FAIpartitionSize = "";
  var $FAIpartitionType = "";
  var $FAIstate = "";
  var $FAIpartitionFlags = "";
  
  var $FAIdiskType = "disk";

  var $FAIpartitionSizeType = "fixed";
  var $sizeStart = 0;
  var $sizeStop  = 0;
  var $sizeStart_Type = "MB";
  var $sizeStop_Type  = "MB";
   
  var $bootable = false; 
  var $resize = false;
  var $preserve = false;
  var $preserveType = "always";
  var $encrypted = false;

  var $status = "";
  var $raidDevices = array();

  function __construct($config, $object, $parent,$type)
  {

    $this->parent = $parent;
    $this->status = "new";
    $this->FAIdiskType = $type;
    
 
    // Load attributes from existing partition 
    if($object){

      foreach($this->attributes as $attr){
        if(isset($object[$attr])){
          $this->$attr = $object[$attr];
        }
      }

      $this->status = $object['status'];

      if($type == "disk" || $type =="lvm"){

        /* Prepare size attribute 
         * This attribute may represent a range, a fixed value 
         *  or a percentage.
         * fixed is just a number   * 500MB
         * range                    * 500MB-1TB
         * remaining                * -
         */
        // Fixed
        if(preg_match("/^[0-9]*(KB|MB|GB|TB|PB|%)$/",$this->FAIpartitionSize)){
          $this->sizeStart = preg_replace("/^([0-9]*)(KB|MB|GB|TB|PB|%)$/","\\1",$this->FAIpartitionSize);
          $this->sizeStart_Type = preg_replace("/^([0-9]*)(KB|MB|GB|TB|PB|%)$/","\\2",$this->FAIpartitionSize);
          $this->FAIpartitionSizeType = "fixed";
        }else

        // Dynamic range
        if(preg_match("/^[0-9]*(KB|MB|GB|TB|PB|%)-[0-9]*(KB|MB|GB|TB|PB|%)$/",$this->FAIpartitionSize)){
          $this->sizeStart = preg_replace("/^([0-9]*)(KB|MB|GB|TB|PB|%).*$/","\\1",$this->FAIpartitionSize);
          $this->sizeStart_Type = preg_replace("/^([0-9]*)(KB|MB|GB|TB|PB|%).*$/","\\2",$this->FAIpartitionSize);
          $this->sizeStop = preg_replace("/^[^\-]*\-([0-9]*)(KB|MB|GB|TB|PB|%)$/","\\1",$this->FAIpartitionSize);
          $this->sizeStop_Type = preg_replace("/^[^\-]*\-([0-9]*)(KB|MB|GB|TB|PB|%)$/","\\2",$this->FAIpartitionSize);
          $this->FAIpartitionSizeType = "dynamic";
        }else

        // Dynamic range
        if(preg_match("/^\-$/",$this->FAIpartitionSize)){
          $this->FAIpartitionSizeType = "remaining";

        }

        /* Check for encrypted partitions
         */
        if(preg_match("/:encrypt$/",$this->FAImountPoint)){
          $this->FAImountPoint = preg_replace("/:encrypt/","",$this->FAImountPoint);
          $this->encrypted = TRUE;
        }
    
      }elseif($type == "raid"){
    
        $usedDisks = split(",",$this->FAIpartitionSize);
        foreach($usedDisks as $disk){
          $name = preg_replace("/:.*$/","",$disk);
          $spare = preg_match("/:spare/",$disk);
          $missing = preg_match("/:missing/",$disk);
          $this->raidDevices[$name] = 
            array(
                "name" => $name, 
                "spare" => $spare, 
                "missing" => $missing);
        }
      }
    }
  }

  function execute()
  {
    plugin::execute();
    $smarty = get_smarty();

    // Remove partition
    if(isset($_POST['addPhysicalPartition']) && isset($_POST['physicalPartitionAdd'])){
      $name = $_POST['physicalPartitionAdd'];
      $this->raidDevices[$name] = array("name" => $name,"spare"=>false,"missing"=>false);     
    }

    // Create a list of selectable partition types
    if($this->FAIdiskType == "disk"){
      $types  = array(
          "primary" => _("Primary"),
          "secondary" => _("Secondary"));
    }elseif($this->FAIdiskType == "raid"){
      $types  = array(
          "raid0" => _("Raid 0"),
          "raid1" => _("Raid 1"),
          "raid5" => _("Raid 5"),
          "raid6" => _("Raid 6"));
    }else{
      $types = "";
    }
     
    // Create a list of all size options
    $partitionSizeTypes  = array(
        "fixed"     => _("Fixed"),
        "dynamic"     => _("Dynamic"),
        "remaining" => _("Use remaining space")
        );

    // Create a list of all size options
    $sizeTypes  = array(
        "KB"      => _("KB"),
        "MB"      => _("MB"),
        "GB"      => _("GB"),
        "TB"      => _("TB"),
        "PB"      => _("PB"),
        "%"      => _("Percent")
        );

    // Preserve types 
    $preserveTypes = array(
        "always" => _("Always"),
        "reinstall" => _("Reinstall"));

    // File system types.  
    $FAIfsTypes = array(
        "swap" => _("Swap"),
        "ext2" => _("Ext 2"),
        "ext3" => _("Ext 3"),
        "ext4" => _("Ext 4"),
        "reiserfs" => _("Reiser fs"),
        "xfs" => _("Xfs"),
        "btrfs" => _("Btrfs"));

    $smarty->assign("partitionTypes", $types);
    $smarty->assign("partitionSizeTypes", $partitionSizeTypes);
    $smarty->assign("FAIpartitionSizeType", $this->FAIpartitionSizeType);
    $smarty->assign("sizeTypes", $sizeTypes);

    $smarty->assign("sizeStart_Type", $this->sizeStart_Type);
    $smarty->assign("sizeStop_Type", $this->sizeStop_Type);
    $smarty->assign("sizeStart", $this->sizeStart);
    $smarty->assign("sizeStop", $this->sizeStop);

    $smarty->assign("preserveTypes", $preserveTypes);
    $smarty->assign("preserveType", $this->preserveType);

    $smarty->assign("FAIfsTypes", $FAIfsTypes);
    $smarty->assign("cn", $this->cn);

    $smarty->assign("plist",$this->getRaidlist());
    $smarty->assign("physicalPartitionList",$this->getPartitionlist());

    foreach($this->attributes as $attr){
      $smarty->assign($attr,$this->$attr);
    }
    return($smarty->fetch(get_template_path("faiPartition.tpl", TRUE, dirname(__FILE__))));
  }

    
  function getPartitionList()
  {
    $array = array();  
    foreach($this->parent->parent->disks as $disk){
      if($disk['FAIdiskType'] != "raid"){
        foreach($disk['partitions'] as $key => $part){
          $name = $part['cn'];
          if(!isset($this->raidDevices[$name])){
            $array[$name] = $name." (".$disk['cn'].")";
          }
        }
      }
    }
    return($array);
  }


  function getRaidList()
  {
    $divlist = new divSelectBox("RaidList");
   
    $disks = $this->parent->parent->disks;
    $objs = array();
    foreach($disks as $disk){
      if($disk['FAIdiskType'] != "raid"){
        foreach($disk['partitions'] as $id => $part){
          $objs[$part['cn']] = $part;
        }
      }
    }
  
    $list = array();
    foreach($this->raidDevices as $device){
      $list[$device['name']] = $device['name'];

      if(isset($objs[$device['name']]['FAIpartitionSize'])){
        $list[$device['name']].= _("Size").":&nbsp;";
        $list[$device['name']].= $objs[$device['name']]['FAIpartitionSize'];
      }

      $list[$device['name']].= " &nbsp; "._("Options").": ";
      if($device['spare']){
        $list[$device['name']].= "&nbsp;"._("Spare")."&nbsp;";
      }
      if($device['missing']){
        $list[$device['name']].= "&nbsp;"._("Missing")."&nbsp;";
      }
    }
    return($list);
  }

  function save_object()
  {
    if(isset($_POST['faiPartition'])){
      foreach($this->attributes as $attr){
        if(isset($_POST[$attr])){
          $this->$attr = get_post($attr);
        }
      }
      foreach(array("FAIpartitionSizeType","sizeStart","sizeStop","sizeStart_Type","sizeStop_Type") as $attr){
        if(isset($_POST[$attr])){
          $this->$attr = get_post($attr);
        }
      }
      foreach(array("bootable","preserve","resize","encrypted") as $attr){
        if(isset($_POST[$attr])){
          $this->$attr = TRUE;
        }else{
          $this->$attr = FALSE;
        }
      }

      // Remove partition
      if(isset($_POST['delPhysicalPartition']) && isset($_POST['physicalPartition'])){
        unset($this->raidDevices[$_POST['physicalPartition']]); 
      }

      // Toggle spare flag for partition entries
      if(isset($_POST['toggleSpare']) && isset($_POST['physicalPartition'])){
        $this->raidDevices[$_POST['physicalPartition']]['spare'] = 
            !$this->raidDevices[$_POST['physicalPartition']]['spare'];
      }

      // Toggle missing flag for partition entries
      if(isset($_POST['toggleMissing']) && isset($_POST['physicalPartition'])){
        $this->raidDevices[$_POST['physicalPartition']]['missing'] = 
            !$this->raidDevices[$_POST['physicalPartition']]['missing'];
      }
    } 
  }


  function check()
  {
    $msgs = plugin::check();
    
    // Check the given partition size.
    if($this->FAIdiskType == "disk" || $this->FAIdiskType == "lvm"){
      if($this->FAIpartitionSizeType == "fixed" || $this->FAIpartitionSizeType == "dynamic"){ 
        if(!is_numeric($this->sizeStart)){
          $msgs[] = msgPool::invalid(_("Partition size"),$this->sizeStart,"/[0-9]/i");
        }
      }
      if($this->FAIpartitionSizeType == "dynamic"){ 
        if(!is_numeric($this->sizeStop)){
          $msgs[] = msgPool::invalid(_("Partition size"),$this->sizeStop,"/[0-9]/i");
        }

        $mp = array(
                    "%"  => 1,
                    "KB" => pow(1024,0),
                    "MB" => pow(1024,1),
                    "GB" => pow(1024,2),
                    "TB" => pow(1024,3),
                    "PB" => pow(1024,4));
        $res1 = $this->sizeStart * $mp[$this->sizeStart_Type];
        $res2 = $this->sizeStop * $mp[$this->sizeStop_Type];
        if($res1 > $res2){
          $msgs[] = msgPool::toobig(_("Partition start size"), "'"._("Partition stop size")."'");
        }
      }
    }
    if($this->FAIdiskType == "raid"){
      #FIME raid checks missing
      echo "Add raid checks here, disk combinations are not verified right now.";
    }

    // check mount point 
    if($this->FAIfsType != "swap"){
      if(!preg_match("#^/#",$this->FAImountPoint)){
        $msgs[] = msgPool::invalid(_("Mount point"));
      }
    }

    return($msgs);
  }


  function save()
  {
    $ret = array();
    foreach($this->attributes as $attr){
      $ret[$attr] = $this->$attr;
    }

    // Save partition size
    if($this->FAIdiskType == "disk" || $this->FAIdiskType == "lvm"){ 
      switch($this->FAIpartitionSizeType){
        case 'fixed' : 
          $ret['FAIpartitionSize'] = $this->sizeStart.$this->sizeStart_Type;break; 
        case 'dynamic' : 
          $ret['FAIpartitionSize'] = $this->sizeStart.$this->sizeStart_Type."-".  
            $this->sizeStop.$this->sizeStop_Type;break; 
        case 'remaining' : 
          $ret['FAIpartitionSize'] = "-";break; 
        default: trigger_error("Unknown partition size!");
      }

      // Add encryption flag to partition mount point
      if($this->encrypted){
        $ret['FAImountPoint'] .= ":encrypt";
      }

    }elseif($this->FAIdiskType == "raid"){
      $ret['FAIpartitionSize'] = "";
      foreach($this->raidDevices as $device){
        $ret['FAIpartitionSize'] .= $device['name'];
        if($device['spare']){
          $ret['FAIpartitionSize'] .= ":spare";
        }
        if($device['missing']){
          $ret['FAIpartitionSize'] .= ":missing";
        }
        $ret['FAIpartitionSize'] .= ",";
      }
      $ret['FAIpartitionSize'] = trim($ret['FAIpartitionSize'],",");
    }
    $ret['status'] = $this->status;

    if($this->FAIfsType == "swap"){
      $ret['FAImountPoint'] = "swap";
    }
  
    return($ret);
  }
}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>