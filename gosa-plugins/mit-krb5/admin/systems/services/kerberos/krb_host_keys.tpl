{if !$is_service_key}
<p class="seperator">&nbsp;</p>
{/if}
<h2><img class="center" alt="" src="images/lists/locked.png" align="middle">	{t}Host key{/t}</h2>
<table style="">
{foreach from=$server_list item=item key=key}
	<tr>
		<td style="padding-right:50px;">{$item.REALM}</td>
		<td>
			{if $item.PRESENT}
				<img src='images/empty.png' class="center">
				<input type='image' class='center' name='recreate_{$key}'
					src='images/lists/reload.png'>
				<input type='image' class='center' name='remove_{$key}'
					src='images/lists/trash.png'>
			{else}
				<input type='image' class='center' name='create_{$key}'
					src='images/lists/new.png'>
				<img src='images/empty.png' class="center">
				<img src='images/empty.png' class="center">
			{/if}
		</td>
	</tr>
	<tr>
		<td><i>{t}Keys for this realm{/t}:</i></td>
		<td>{$item.USED}</td>
	</tr>
{/foreach}
</table>
{if $is_service_key}
<p class="seperator">&nbsp;</p>
{/if}