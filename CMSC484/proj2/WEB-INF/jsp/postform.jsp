<%@ page contentType="text/html; charset=UTF-8" language="java" %>
<jsp:useBean id="formData" type="org.thestaticvoid.cmsc484blog.PostArticleFormData" scope="request" />
<form action="org.thestaticvoid.cmsc484blog.PostArticleServlet" method="post">
	<table>
		<tr>
			<td>Title</td>
			<td><input type="text" size="40" name="title" value="<jsp:getProperty name="formData" property="title" />" /></td>
		</tr>
		<tr>
			<td>Content</td>
			<td><textarea rows="10" cols="40" name="content"><jsp:getProperty name="formData" property="content" /></textarea></td>
		</tr>
	</table>

	<input type="hidden" name="aid" value="<%= request.getParameter("aid") %>" />
	<input type="submit" value="Post" />
</form>
<jsp:useBean id="error" type="org.thestaticvoid.cmsc484blog.Error" scope="request" />
<div style="color: red"><jsp:getProperty name="error" property="error" /></div>
