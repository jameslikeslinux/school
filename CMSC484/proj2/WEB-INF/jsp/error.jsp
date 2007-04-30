<%@ page contentType="text/plain; charset=UTF-8" language="java" import="java.io.*" %>
<%
	Exception e = (Exception) request.getAttribute("e");
	e.printStackTrace(new PrintWriter(out));
%>
