<%@ page contentType="text/html; charset=UTF-8" language="java" import="org.thestaticvoid.cmsc484blog.*" %>
<%
	Article[] articles = (Article[]) request.getAttribute("articles");
	for (int i = 0; i < articles.length; i++) {
%>
		<h4><a href="org.thestaticvoid.cmsc484blog.ViewArticlesServlet?aid=<%= articles[i].getId() %>"><%= articles[i].getTitle() %></a></h4>
		<h5><%= articles[i].getUsername() %></h5>
		<h6><%= articles[i].getTimestamp() %></h6>
		<p><%= articles[i].getContent() %></p>
<%
	}
%>
