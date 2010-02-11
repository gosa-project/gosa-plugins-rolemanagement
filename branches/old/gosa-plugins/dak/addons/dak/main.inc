<?php
/*
  This code is part of GOsa (https://gosa.gonicus.de)
  Copyright (C) 2003  Cajus Pollmeier

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
if (!$remove_lock){
	if (!session::is_set('dak_plug') || (isset($_GET['reset']) && $_GET['reset'] == 1)){
		session::set('dak_plug',new dakplug($config));
	}
	$dak_plug = session::get('dak_plug');

	/* Execute formular */
	$display= $dak_plug->save_object();
	$display= $dak_plug->execute ();
	$display.= "<input type=\"hidden\" name=\"ignore\">\n";

	/* Page header*/
	$display= print_header(get_template_path($dak_plug->plIcon), _("DAK")).$display;

	/* Store changes  in session */
	session::set('dak_plug',$dak_plug);
}
?>