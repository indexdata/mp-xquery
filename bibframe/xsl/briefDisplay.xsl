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
      <xsl:if test="bf:Work/bf:creator">
	<xsl:variable
	    name="resource" select="bf:Work/bf:creator/@rdf:resource"/>
	<tr>
	  <td>Author</td>
	  <td>
	    <xsl:value-of select="//bf:Person[@rdf:about=$resource]/bf:label"/>
	  </td>
	</tr>
      </xsl:if>
      <xsl:if test="bf:Work/bf:workTitle">
	<xsl:variable
	    name="resource" select="bf:Work/bf:workTitle/@rdf:resource"/>
	<tr>
	  <td>Work Title</td>
	  <td>
	    <xsl:value-of select="//bf:Title[@rdf:about=$resource]/bf:titleValue"/>
	  </td>
	</tr>
      </xsl:if>
      <xsl:if test="//bf:Instance/bf:providerStatement">
      <tr>
	<td>Date/Place</td>
	<td>
	  <xsl:value-of select="//bf:Instance/bf:providerStatement"/>
	</td>
      </tr>
      </xsl:if>
    </table>
  </xsl:template>

  <xsl:template match="/">
    <xsl:call-template name="html"/>
  </xsl:template>

  <xsl:template name="html">
    <html>
      <head>
        <title>
          <xsl:value-of select="//zr:explain/zr:databaseInfo/zr:title"/>
        </title>
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

  <xsl:template match="zr:explain">
    <xsl:call-template name="dbinfo"/>
    <xsl:call-template name="diagnostic"/>
    <xsl:call-template name="searchform"/>
  </xsl:template>

  <xsl:template match="srw:searchRetrieveResponse">
    <h2>Search Results</h2>
    <xsl:call-template name="diagnostic"/>
    <xsl:call-template name="displaysearch"/>
  </xsl:template>

  <xsl:template name="dbinfo">
    <div class="dbinfo">
      <h1><xsl:value-of select="//zr:explain/zr:databaseInfo/zr:title"/>
      </h1>
      <h2><xsl:value-of select="//zr:explain/zr:databaseInfo/zr:description"/>
      </h2>
      <h4>
        <xsl:value-of select="//zr:explain/zr:databaseInfo/zr:author"/>
        <br/>
        <xsl:value-of select="//zr:explain/zr:databaseInfo/zr:history"/>
      </h4>
    </div>
  </xsl:template>

  <xsl:template name="searchform">
    <div class="searchform">
      <form name="searchform"  method="get"> <!-- action=".." -->
        <input type="hidden" name="version" value="1.2"/>
        <input type="hidden" name="operation" value="searchRetrieve"/>
	<input type="hidden" name="stylesheet">
	  <xsl:attribute name="value">
	    <xsl:value-of select="//srw:echoedExplainRequest/srw:stylesheet"/>
	    <xsl:value-of select="//sru:echoedExplainRequest/sru:stylesheet"/>
	  </xsl:attribute>
	</input>
        <div class="query">
          <input type="text" name="query"/>
        </div>
        <div class="parameters">
          <xsl:text>startRecord: </xsl:text>
          <input size="10" type="text" name="startRecord" value="1"/>
          <xsl:text> maximumRecords: </xsl:text>
          <input size="10" type="text" name="maximumRecords" value="5"/>
          <xsl:text> recordSchema: </xsl:text>
          <select name="recordSchema">
          <xsl:for-each select="//zr:schemaInfo/zr:schema">
            <option value="{@name}">
              <xsl:value-of select="zr:title"/>
            </option>
          </xsl:for-each>
          </select>
          <xsl:text> recordPacking: </xsl:text>
          <select name="recordPacking">
            <option value="string">string</option>
            <option value="xml">XML</option>
          </select>

        </div>

        <div class="submit">
          <input type="submit" value="Send Search Request"/>
        </div>
      </form>
    </div>
  </xsl:template>

  <xsl:template name="indexinfo">
     <div class="dbinfo">
       <xsl:for-each
          select="//zr:indexInfo/zr:index[zr:map/zr:name/@set]">
        <xsl:variable name="index">
          <xsl:value-of select="zr:map/zr:name/@set"/>
          <xsl:text>.</xsl:text>
          <xsl:value-of select="zr:map/zr:name/text()"/>
        </xsl:variable>
        <b><xsl:value-of select="$index"/><br/></b>
      </xsl:for-each>
     </div>
  </xsl:template>


  <xsl:template name="relationinfo">
    <!--
      <xsl:variable name="defrel"
                    select="//zr:configInfo/zr:default[@type='relation']"/>
      <b><xsl:value-of select="$defrel"/><br/></b>
      -->
      <xsl:for-each select="//zr:configInfo/zr:supports[@type='relation']">
        <xsl:variable name="rel" select="text()"/>
        <b><xsl:value-of select="$rel"/><br/></b>
      </xsl:for-each>
  </xsl:template>


  <!-- diagnostics -->
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
      <xsl:for-each select="srw:nextRecordPosition">
        <h4>
          <xsl:text>Next Record Position: </xsl:text>
          <xsl:value-of select="."/>
         </h4>
      </xsl:for-each>

      <xsl:for-each select="srw:records">
        <xsl:for-each select="srw:record">
          <div class="record">
            <p>
              <xsl:text>Record: </xsl:text>
              <xsl:value-of select="srw:recordPosition"/>
              <xsl:text> : </xsl:text>
              <xsl:value-of select="srw:recordSchema"/>
              <xsl:text> : </xsl:text>
              <xsl:value-of select="srw:recordPacking"/>
	    </p>
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

	      <form name="fulllink" method="get">
		<input type="hidden" name="version" value="1.2"/>
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
		<input type="hidden" name="stylesheet" value="/xsl/briefDisplay.xsl"/>
		<input type="hidden" name="startRecord">
		  <xsl:attribute name="value">
		    <xsl:value-of select="srw:recordPosition"/>
		  </xsl:attribute>
		</input>
		<input type="hidden" name="maximumRecords" value="1"/>
		<input type="submit">
		  <xsl:attribute name="value">
		    <xsl:text>Full Record </xsl:text>
		    <xsl:value-of select="srw:recordPosition"/>
		  </xsl:attribute>
		</input>
	      </form>
	      <form name="rawlink" method="get">
		<input type="hidden" name="version" value="1.2"/>
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
