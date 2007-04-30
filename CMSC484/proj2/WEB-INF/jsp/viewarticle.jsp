<%@ page contentType="text/html; charset=UTF-8" language="java" import="org.thestaticvoid.cmsc484blog.*" %>
<jsp:useBean id="article" type="org.thestaticvoid.cmsc484blog.Article" scope="request" />
<h4><jsp:getProperty name="article" property="title" /></h4>
<h5><jsp:getProperty name="article" property="username" /></h5>
<h6><jsp:getProperty name="article" property="timestamp" /></h6>
<p><jsp:getProperty name="article" property="content" /></p>
<h3>Comments</h3>
<%
	Article[] comments = article.getComments();
	for (int i = comments.length - 1; i >= 0; i--) {
%>
		<h4><%= comments[i].getTitle() %></h4>
		<h5><%= comments[i].getUsername() %></h5>
		<h6><%= comments[i].getTimestamp() %></h6>
		<p><%= comments[i].getContent() %></p>
<%
	}
%>
<a href="org.thestaticvoid.cmsc484blog.PostArticleServlet?aid=<jsp:getProperty name="article" property="id" />">Comment</a>
