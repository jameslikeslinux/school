<%@ page contentType="text/html; charset=UTF-8" language="java" %>
<jsp:useBean id="userData" type="org.thestaticvoid.cmsc484blog.UserData" scope="session" />
<span>Welcome, <jsp:getProperty name="userData" property="username" />!</span>
<a href="org.thestaticvoid.cmsc484blog.PreferencesServlet">Preferences</a>
<br /><br />
