<%@ page contentType="text/html; charset=UTF-8" language="java" %>
<jsp:useBean id="userData" type="org.thestaticvoid.cmsc484blog.UserData" scope="session" />
<div>Welcome, <jsp:getProperty name="userData" property="username" />!</div>
<a href="org.thestaticvoid.cmsc484blog.PreferencesServlet">Preferences</a>
<a href="org.thestaticvoid.cmsc484blog.LogoutServlet">Logout</a>
<br /><br />
