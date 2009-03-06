<?php
/*
 * This code is part of GOsa (http://www.gosa-project.org)
 * Copyright (C) 2003-2008 GONICUS GmbH
 *
 * ID: $$Id$$
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

class ServiceAddDialog extends plugin{

  var $cli_summary      = "This dialog is used to add services";
  var $cli_description  = "Some longer text\nfor help";
  var $cli_parameters   = array("config"=>"Config object" , "dn"=>"Object dn");

  /* This plugin does not have any ocs */
  var $objectclasses    = array();
  var $parent           = NULL;

  function ServiceAddDialog(&$config,$dn,$parent)
  {
    plugin::plugin($config);
    $this->parent = $parent;
  }

  function execute()
  {
    $smarty = get_smarty();
    $services = $this->parent->getAllUnusedServices();
    natcasesort($services);
    $smarty->assign("Services",$services);
    return($smarty->fetch(get_template_path("ServiceAddDialog.tpl", TRUE,dirname(__FILE__))));
 }

  function check(){ return array();}
  function save_object(){;}
  function save(){}

}
// vim:tabstop=2:expandtab:shiftwidth=2:filetype=php:syntax:ruler:
?>