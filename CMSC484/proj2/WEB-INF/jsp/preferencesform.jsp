<%@ page contentType="text/html; charset=UTF-8" language="java" %>
<jsp:useBean id="formData" type="org.thestaticvoid.cmsc484blog.PreferencesFormData" scope="request" />
<form action="org.thestaticvoid.cmsc484blog.PreferencesServlet" method="post">
	<table>
		<tr>
			<td>Invert Colors</td>
			<td><input type="checkbox" name="invert" <%= formData.getInvert() ? "checked=\"checked\"" : "" %> /></td>
		</tr>
	</table>

	<input type="submit" name="submit" value="Change" />
</form>
