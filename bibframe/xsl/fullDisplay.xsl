<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:srw="http://www.loc.gov/zing/srw/"
		xmlns:sru="http://docs.oasis-open.org/ns/search-ws/sruResponse"
                xmlns:dc="http://www.loc.gov/zing/srw/dcschema/v1.0/"
                xmlns:zr="http://explain.z3950.org/dtd/2.0/"
                xmlns:diag="http://www.loc.gov/zing/srw/diagnostic/"
                xmlns:rdf="http://www.w3.org/1999/02/22-rdf-syntax-ns#"
		xmlns:bf="http://bibframe.org/vocab/"
                version="1.0">

  <xsl:output method="html" version="1.0" encoding="UTF-8" indent="yes"/>

  <xsl:template match="text()"/>

  <xsl:template match="rdf:RDF">
    <table>
      <xsl:for-each select="bf:*">
	<tr><td><b><xsl:value-of select="name(.)"/>:</b></td></tr>
	<xsl:for-each select="./*">
	  <xsl:variable name="resource" select="./@rdf:resource"/>
	  <xsl:variable name="nodeid" select="./@rdf:nodeID"/>
	  <tr>
	    <td/><td><b><xsl:value-of select="name(.)"/>:</b></td>
	    <td/><td>
	      <xsl:if test="$resource">
		<xsl:variable name="rvalue"
			      select="//*[@rdf:about=$resource]/*[1]"/>
		<xsl:choose>
		  <xsl:when test="$rvalue">
		    <xsl:value-of select="$rvalue"/>
		  </xsl:when>
		  <xsl:otherwise>
		    <xsl:value-of select="$resource"/>
		  </xsl:otherwise>
		</xsl:choose>
	      </xsl:if>
	      <xsl:if test="$nodeid">
		<xsl:variable name="rvalue"
			      select="//*[@rdf:nodeID=$nodeid]/*[1]"/>
		<xsl:choose>
		  <xsl:when test="$rvalue">
		    <xsl:value-of select="$rvalue"/>
		  </xsl:when>
		  <xsl:otherwise>
		    <xsl:value-of select="$nodeid"/>
		  </xsl:otherwise>
		</xsl:choose>
	      </xsl:if>
	      <xsl:value-of select="."/>
	    </td>
	  </tr>
	</xsl:for-each>
      </xsl:for-each>
    </table>
  </xsl:template>

  <xsl:template match="/">
    <xsl:call-template name="html"/>
  </xsl:template>

  <xsl:template name="html">
    <html>
      <head>
        <title>BIBFRAME Full Display</title>
        <link href="css.css" rel="stylesheet"
              type="text/css" media="screen, all"/>
      </head>
      <body>
        <div class="body">
          <xsl:apply-templates/>
        </div>
      </body>
    </html>
  </xsl:template>

  <xsl:template match="srw:searchRetrieveResponse">
    <h2>Search Results</h2>
    <xsl:call-template name="diagnostic"/>
    <xsl:call-template name="displaysearch"/>
  </xsl:template>

  <xsl:template name="diagnostic">
    <xsl:for-each select="//diag:diagnostic">
     <div class="diagnostic">
        <!-- <xsl:value-of select="diag:uri"/> -->
        <xsl:text> </xsl:text>
        <xsl:value-of select="diag:message"/>
        <xsl:text>: </xsl:text>
        <xsl:value-of select="diag:details"/>
      </div>
    </xsl:for-each>
  </xsl:template>

  <xsl:template name="displaysearch">
    <div class="searchresults">
      <xsl:for-each select="srw:numberOfRecords">
        <h4>
          <xsl:text>Number of Records: </xsl:text>
          <xsl:value-of select="."/>
        </h4>
      </xsl:for-each>
      <xsl:for-each select="srw:records">
        <xsl:for-each select="srw:record">
          <div class="record">
            <h4>
              <xsl:text>Record </xsl:text>
              <xsl:value-of select="srw:recordPosition"/>
	    </h4>
            <p>
	      <xsl:if test="srw:recordPacking='string'">
		<pre>
		  <xsl:value-of select="srw:recordData"/>
		</pre>
	      </xsl:if>
	      <xsl:if test="srw:recordPacking='xml'">
		<xsl:choose>
		  <xsl:when test="srw:recordSchema='marcxml'">
		    <xsl:text>MARCXML</xsl:text>
		  </xsl:when>
		  <xsl:when test="srw:recordSchema='bibframe'">
		    <xsl:apply-templates select="srw:recordData"/>
		  </xsl:when>
		</xsl:choose>
	      </xsl:if>

	      <form name="rawlink" method="get">
		<input type="hidden" name="version">
		  <xsl:attribute name="value">
		    <xsl:value-of
			select="//srw:echoedSearchRetrieveRequest/srw:version"/>
		  </xsl:attribute>
		</input>
		<input type="hidden" name="operation" value="searchRetrieve"/>
		<input type="hidden" name="query">
		  <xsl:attribute name="value">
		    <xsl:value-of
			select="//srw:echoedSearchRetrieveRequest/srw:query"/>
		  </xsl:attribute>
		</input>
		<input type="hidden" name="recordPacking">
		  <xsl:attribute name="value">
		    <xsl:value-of select="srw:recordPacking"/>
		  </xsl:attribute>
		</input>
		<input type="hidden" name="recordSchema">
		  <xsl:attribute name="value">
		    <xsl:value-of select="srw:recordSchema"/>
		  </xsl:attribute>
		</input>
		<input type="hidden" name="startRecord">
		  <xsl:attribute name="value">
		    <xsl:value-of select="srw:recordPosition"/>
		  </xsl:attribute>
		</input>
		<input type="hidden" name="maximumRecords" value="1"/>
		<input type="submit">
		  <xsl:attribute name="value">
		    <xsl:text>Raw Record </xsl:text>
		    <xsl:value-of select="srw:recordPosition"/>
		  </xsl:attribute>
		</input>
	      </form>
	    </p>
          </div>
        </xsl:for-each>
      </xsl:for-each>
    </div>
  </xsl:template>

</xsl:stylesheet>
