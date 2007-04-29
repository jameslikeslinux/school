<%@ page contentType="text/html; charset=UTF-8" language="java" %>
<jsp:useBean id="formData" type="org.thestaticvoid.cmsc484blog.CreateAccountFormData" scope="request" />
<form action="org.thestaticvoid.cmsc484blog.CreateAccountServlet" method="post">
	<table>
		<tr>
			<td>Username</td>
			<td><input type="text" name="username" value="<jsp:getProperty name="formData" property="username" />" /></td>
		</tr>
		<tr>
			<td>Password</td>
			<td><input type="password" name="password1" /></td>
		</tr>
		<tr>
			<td>Again</td>
			<td><input type="password" name="password2" /></td>
		</tr>
		<tr>
			<td>Real name</td>
			<td><input type="text" name="name" value="<jsp:getProperty name="formData" property="name" />" /></td>
		</tr>
	</table>

	<input type="submit" value="Create" />
</form>
<jsp:useBean id="error" type="org.thestaticvoid.cmsc484blog.Error" scope="request" />
<div style="color: red"><jsp:getProperty name="error" property="error" /></div>
