<?php

namespace GosaRoleManagement\admin\roleManagement;

$class_mapping['roleManagement'] = dirname(__FILE__).'RoleManagement.php';

bindtextdomain("roleManagement", dirname(dirname(__FILE__)) . "/locale/compiled");

function __($GETTEXT) {
    return dgettext("roleManagement", $GETTEXT);
}
