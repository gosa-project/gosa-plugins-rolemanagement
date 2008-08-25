
<h2>Opsi host</h2>

{if $init_failed}

<font style='color: #FF0000;'>{msgPool type=siError p=$message}</font>

<input type='submit' name='reinit' value="{t}Retry{/t}">

{else}

<table style="width: 100%;">
 <tr>
  <td>
   <table>
    {if $parent_mode}
     <tr>
      <td>{t}Name{/t}</td>
      <td><input type='text' name='hostId' value='{$hostId}'></td>
     </tr>
    {/if}
    <tr>
     <td>{t}Description{/t}</td>
     <td><input type='text' name='description' value='{$description}'></td>
    </tr>
    <tr>
     <td>{t}Notes{/t}</td>
     <td><input type='text' name='note' value='{$note}'></td>
    </tr>
   </table>
  </td>
  <td style='vertical-align: top;'>
   <table>
    <tr>
     <td>{t}MAC address{/t}</td>
     <td><input type='text' name='mac' value='{$mac}'></td>
    </tr>
    <tr>
     <td>{t}Boot product{/t}</td>
     <td>
      <select name="opsi_netboot_product">
		{foreach from=$ANP item=item key=key}
			<option {if $key == $SNP} selected {/if} value="{$key}">{$key}</option>
		{/foreach}
      </select>
     </td>
    </tr>
   </table>
  </td>
 </tr>
 <tr>
  <td colspan="2">
   <p class='seperator'>&nbsp;</p>
  </td>
 </tr>
 <tr>
  <td style="width:50%;"><h2>Installed products</h2>
	{$divSLP}
  </td>
  <td style="width:50%;"><h2>Available products</h2>
	{$divALP}
  </td>
 </tr>
</table> 
<input type='hidden' name='opsigeneric_posted' value='1'>
{/if}
