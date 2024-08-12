<?php

namespace GosaRoleManagement\admin\roleManagement;

bindtextdomain("roleManagement", dirname(dirname(__FILE__)) . "/locale/compiled");

function __($GETTEXT) {
    return dgettext("roleManagement", $GETTEXT);
}
