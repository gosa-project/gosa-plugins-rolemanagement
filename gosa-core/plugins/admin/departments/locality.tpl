<!--////////////////////
	//	LOCALITY (l)
    //////////////////// -->

<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
 <tr>
   <td style="vertical-align:top; width:50%">
     <h2><img class="center" alt="" align="middle" src="images/rightarrow.png"> {t}Properties{/t}</h2>
     <table summary="">
      <tr>
       <td><LABEL for="l">{t}Locality name{/t}</LABEL>{$must}</td>
       <td>
{render acl=$lACL}
    	<input id="l" name="l" size=25 maxlength=60 value="{$l}" title="{t}Name of locality to create{/t}">
{/render}
       </td>
      </tr>
      <tr>
       <td><LABEL for="description">{t}Description{/t}</LABEL>{$must}</td>
       <td>
{render acl=$descriptionACL}
        <input id="description" name="description" size=25 maxlength=80 value="{$description}" title="{t}Descriptive text for department{/t}">
{/render}
       </td>
      </tr>
      <tr>
        <td><LABEL for="base">{t}Base{/t}</LABEL>{$must}</td>
        <td>
{render acl=$baseACL}
         <select id="base" size="1" name="base" title="{t}Choose subtree to place department in{/t}"> 
          {html_options options=$bases selected=$base_select}
         </select>
{/render}

{render acl=$baseACL disable_picture='images/lists/folder_grey.png'}
        <input type="image" name="chooseBase" src="images/lists/folder.png" class="center" title="{t}Select a base{/t}">
{/render}
	  </td>
	 </tr>
	</table>
  </td>
 </tr>
</table>
<p class='seperator'>&nbsp;</p>
<table summary="" style="width:100%; vertical-align:top; text-align:left;" cellpadding=4>
 <tr>
   <td style="vertical-align:top; width:100%">
     <h2><img class="center" alt="" align="middle" src="images/lists/locked.png"> {t}Administrative settings{/t}</h2>
{render acl=$gosaUnitTagACL}
     <input id="unitTag" type=checkbox name="unitTag" value="1" {$unitTag}><label for="unitTag">{t}Tag department as an independent administrative unit{/t}</label>
{/render}
   </td>
  </tr>
</table>
<input type='hidden' name='dep_generic_posted' value='1'>
