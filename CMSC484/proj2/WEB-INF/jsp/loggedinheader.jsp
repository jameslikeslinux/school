<%@ page contentType="text/html; charset=UTF-8" language="java" %>
<jsp:useBean id="username" type="org.thestaticvoid.cmsc484blog.Username" scope="session" />
<span>Welcome, <jsp:getProperty name="username" property="username" />!</span>
<a href="org.thestaticvoid.cmsc484blog.PreferencesServlet">Preferences</a>
<br /><br />
