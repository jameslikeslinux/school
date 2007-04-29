<%@ page contentType="text/html; charset=UTF-8" language="java" %>
<jsp:useBean id="formData" type="org.thestaticvoid.cmsc484blog.LoginFormData" scope="request" />
<form action="org.thestaticvoid.cmsc484blog.LoginServlet" method="post">
	<table>
		<tr>
			<td>Username</td>
			<td><input type="text" name="username" value="<jsp:getProperty name="formData" property="username" />" /></td>
		</tr>
		<tr>
			<td>Password</td>
			<td><input type="password" name="password" /></td>
		</tr>
	</table>

	<input type="submit" value="Login" />
</form>
<jsp:useBean id="error" type="org.thestaticvoid.cmsc484blog.Error" scope="request" />
<div style="color: red"><jsp:getProperty name="error" property="error" /></div>
