

# Hadoop MapReduce Tutorial #

In this section, we'll walk you through two Hadoop MapReduce examples, one using Java MapReduce and the other using Hadoop streaming.  Both examples do the same thing and illustrate how to run MapReduce jobs that read from and write to a table in Hypertable.

## Setup ##

This program assumes there exists a table called _wikipedia_ that has been loaded with a Wikipedia dump.  It reads the _article_ column, tokenizes it, and populates the _word_ column of the same table.  Each unique word in the article turns into a qualified column and the value is the number of times the word appears in the article.

The first step is to get Hadoop up and running.  After that, Hypertable needs to be launched on top of Hadoop, which can be done through Capistrano as described in [#Up\_and\_Running\_with\_HDFS](#Up_and_Running_with_HDFS.md), or if just running a local instance of Hadoop, can be launched with:

```
/opt/hypertable/current/bin/ht start-all-servers hadoop
```

Download and uncompress the Wikipedia dump.  This dump has been converted into `.tsv` format, digestible by LOAD DATA INFILE.

```
wget https://s3.amazonaws.com/hypertable-data/wikipedia.tsv.gz
```

Create the _wikipedia_ table by executing the following commands in the hypertable shell:

```
CREATE NAMESPACE test;
USE test;
DROP TABLE IF EXISTS wikipedia;
CREATE TABLE wikipedia (
       title,
       id,
       username,
       article,
       word
);
```

## Java MapReduce Example ##

In this example, we'll be running the `WikipediaWordCount.java` program which can be found in the  `examples/java/org/hypertable/examples/hadoop/mapreduce` directory of the source archive.  It is also compiled into the `hadoop-*-examples.jar` file, contained in the binary packages.  The following is a link to the code:

[WikipediaWordCount.java](http://www.hypertable.org/pub/code-examples/WikipediaWordCount.java.txt)

Use the following command to load the Wikipedia dump into the _wikipedia_ table:

```
USE test;
load data infile 'wikipedia.tsv.gz' into table wikipedia;
```

To get an idea of what the data looks like, try the following select:

```
hypertable> select * from wikipedia where row =^ "Addington";
Addington, Buckinghamshire	title	Addington, Buckinghamshire
Addington, Buckinghamshire	id	377201
Addington, Buckinghamshire	username	Roleplayer
Addington, Buckinghamshire	article	{{infobox UK place \n|country = England\n|latitude=51.95095\n|longitude=-0.92177\n|official_name= Addington\n| population = 145 ...
```

Now run the WikipediaWordCount MapReduce program:

```
/opt/hadoop/current/bin/hadoop jar \
    /opt/hypertable/current/lib/java/hypertable-0.9.5.0-examples.jar \  
    org.hypertable.examples.WikipediaWordCount \
    --namespace=test --columns=article
```

To verify that it worked, try selecting for the the _word\_column:_

```
hypertable> select word from wikipedia where row =^ "Addington";
[...]
Addington, Buckinghamshire	word:A	1
Addington, Buckinghamshire	word:Abbey	1
Addington, Buckinghamshire	word:Abbotts	1
Addington, Buckinghamshire	word:According	1
Addington, Buckinghamshire	word:Addington	6
Addington, Buckinghamshire	word:Adstock	1
Addington, Buckinghamshire	word:Aston	1
Addington, Buckinghamshire	word:Aylesbury	3
Addington, Buckinghamshire	word:BUCKINGHAM	1
Addington, Buckinghamshire	word:Bayeux	2
Addington, Buckinghamshire	word:Bene	1
Addington, Buckinghamshire	word:Bishop	1
[...]
```

## Hadoop Streaming ##

In this example, we'll be running a Hadoop streaming MapReduce job that uses a Bash script as the mapper and a Bash script as the reducer.  The mapper script (`tokenize-article.sh`) and the reducer script (`reduce-word-counts.sh`) are show below.

**Mapper script (`tokenize-article.sh`)**

```
#!/usr/bin/env bash

IFS="	"
read name column article

while [ $? == 0 ] ; do

  if [ "$column" == "article" ] ; then

    # Strip punctuation
    stripped_article=`echo $article | awk 'BEGIN { FS="\t" } { print $NF }' | tr "\!\"#\$&'()*+,-./:;<=>?@[\\\\]^_\{|}~" " " | tr -s " "` ;

    # Split article into words
    echo $stripped_article | awk -v name="$name" 'BEGIN { article=name; FS=" "; } { for (i=1; i<=NF; i++) printf "%s\tword:%s\t1\n", article, $i; }' ;

  fi

  # Read another line
  read name column article

done
exit 0
```

**Reducer script (`reduce-word-counts.sh`)**

```
#!/usr/bin/env bash

last_article=
last_word=
let total=0

IFS="	"
read article word count

while [ $? == 0 ] ; do
    if [ "$article" == "$last_article" ] && [ "$word" == "$last_word" ] ; then
        let total=$count+total
    else
        if [ "$last_word" != "" ]; then
            echo "$last_article	$last_word	$total"
        fi
        let total=$count
        last_word=$word
        last_article=$article
    fi
    read article word count
done

if [ $total -gt 0 ] ; then
    echo "$last_article	$last_word	$total"
fi
exit 0
```

To populate the word column of the wikipedia table by tokenizing the article column using the above mapper and reduce script, issue the following command:

```
hadoop jar /usr/lib/hadoop-0.20/contrib/streaming/hadoop-streaming-0.20.2-cdh3u0.jar \
-libjars /opt/hypertable/current/lib/java/hypertable-0.9.5.0.pre4.jar,/opt/hypertable/current/lib/java/libthrift-0.6.0.jar \
-Dhypertable.mapreduce.namespace=test \
-Dhypertable.mapreduce.input.table=wikipedia \
-Dhypertable.mapreduce.output.table=wikipedia \
-mapper /home/doug/tokenize-article.sh \
-combiner /home/doug/reduce-word-counts.sh \
-reducer /home/doug/reduce-word-counts.sh \
-file /home/doug/tokenize-article.sh \
-file /home/doug/reduce-word-counts.sh \
-inputformat org.hypertable.hadoop.mapred.TextTableInputFormat \
-outputformat org.hypertable.hadoop.mapred.TextTableOutputFormat \
-input wikipedia -output wikipedia
```

### Job Configuration Properties ###

The following table lists the job configuration properties that are supported by the Hypertable streaming MapReduce interface.  They can be supplied to a MapReduce job with `-D<property>=<value>` arguments.

| **Property** | **Description** | **Examples** |
|:-------------|:----------------|:-------------|
| `hypertable.mapreduce.namespace` | Namespace for both input and output table | `/test`      |
| `hypertable.mapreduce.input.namespace` | Namespace for input table | `/test/intput` |
| `hypertable.mapreduce.input.table` | Input table name | `wikipedia`  |
| `hypertable.mapreduce.input.scan_spec.columns` | Comma-separated list of input columns | `id,title`   |
| `hypertable.mapreduce.input.scan_spec.options` | Input WHERE clause options | `MAX_VERSIONS 1 KEYS_ONLY CELL_LIMIT 2` |
| `hypertable.mapreduce.input.scan_spec.row_interval` | Input row interval | `Dog <= ROW < Kitchenette` |
| `hypertable.mapreduce.input.scan_spec.timestamp_interval` | Cell timestamp interval filter | `TIMESTAMP < 2010-08-03 12:30:00` <br> <code>TIMESTAMP &gt;= 2010-08-03 12:30:00</code> <br> <code>2010-08-01 &lt;= TIMESTAMP &lt;= 2010-08-09</code> <br>
<tr><td> <code>hypertable.mapreduce.input.include_timestamps</code> </td><td> Emit integer timestamp as the<br>1st field (nanoseconds since epoch) </td><td> <code>true</code> </td></tr>
<tr><td> <code>hypertable.mapreduce.output.namespace</code> </td><td> Namespace for output table </td><td> <code>/test/output</code> </td></tr>
<tr><td> <code>hypertable.mapreduce.output.table</code> </td><td> Output table name </td><td> <code>wikipedia</code> </td></tr>
<tr><td> <code>hypertable.mapreduce.output.mutator_flags</code> </td><td> flags parameter passed to open_mutator() </td><td> <code>1</code> </td></tr>
<tr><td> <code>hypertable.mapreduce.output.mutator_flush_interval</code> </td><td> flush_interval parameter passed to<br>open_mutator() </td><td> <code>1000</code> </td></tr></tbody></table>

<h3>Column Selection</h3>

To run a MapReduce job over a subset of columns from the input table, specify a comma separated list of columns in the <code>hypertable.mapreduce.input.scan_spec.columns</code> Hadoop configuration property.<br>
<br>
<b>Example:</b>

<pre><code>hadoop jar /usr/lib/hadoop-0.20/contrib/streaming/hadoop-streaming-0.20.2-cdh3u0.jar \<br>
-libjars /opt/hypertable/current/lib/java/hypertable-0.9.5.0.pre4.jar,/opt/hypertable/current/lib/java/libthrift-0.6.0.jar \<br>
-Dhypertable.mapreduce.namespace=test \<br>
-Dhypertable.mapreduce.input.table=wikipedia \<br>
-Dhypertable.mapreduce.input.scan_spec.columns="id,title" \<br>
-mapper /bin/cat -reducer /bin/cat \<br>
-inputformat org.hypertable.hadoop.mapred.TextTableInputFormat \<br>
-input wikipedia -output columns<br>
</code></pre>

<h3>Timestamps</h3>

<b>Timestamp Predicate</b>.  To filter the input table with a timestamp predicate, specify the timestamp predicate in the <code>hypertable.mapreduce.input.scan_spec.timestamp_interval</code> Hadoop configuration property.  The timestamp predicate is specified using the same format as the timestamp predicate in the <code>WHERE</code> clause of the <code>SELECT</code> statement.  For example:<br>
<br>
<ul><li><code>TIMESTAMP &lt; 2010-08-03 12:30:00</code>
</li><li><code>TIMESTAMP &gt;= 2010-08-03 12:30:00</code>
</li><li><code>2010-08-01 &lt;= TIMESTAMP &lt;= 2010-08-09</code></li></ul>

<b>Preserving Timestamps</b>.  To preserve the timestamps from the input table, set the <code>hypertable.mapreduce.input.include_timestamps</code> Hadoop configuration property to <i>true</i>.  This will cause the TextTableInputFormat class to produce an additional field (field 0) that represents the timestamp as nanoseconds since the epoch.<br>
<br>
<b>Example:</b>

<pre><code>hadoop jar /usr/lib/hadoop-0.20/contrib/streaming/hadoop-streaming-0.20.2-cdh3u0.jar \<br>
-libjars /opt/hypertable/current/lib/java/hypertable-0.9.5.0.pre4.jar,/opt/hypertable/current/lib/java/libthrift-0.6.0.jar \<br>
-Dhypertable.mapreduce.namespace=test \<br>
-Dhypertable.mapreduce.input.table=wikipedia \<br>
-Dhypertable.mapreduce.output.table=wikipedia2 \<br>
-Dhypertable.mapreduce.input.scan_spec.columns="id,title" \<br>
-Dhypertable.mapreduce.input.scan_spec.timestamp_interval="2010-08-01 &lt;= TIMESTAMP &lt;= 2010-08-09" \<br>
-Dhypertable.mapreduce.input.include_timestamps=true \<br>
-mapper /bin/cat -reducer /bin/cat \<br>
-inputformat org.hypertable.hadoop.mapred.TextTableInputFormat \<br>
-outputformat org.hypertable.hadoop.mapred.TextTableOutputFormat \<br>
-input wikipedia -output wikipedia2<br>
</code></pre>

<h3>Options</h3>

A subset of the WHERE clause options of the HQL SELECT statement can be specified by supplying the options with the <code>hypertable.mapreduce.input.scan_spec.options</code> Hadoop configuration property.  The following options are supported:<br>
<br>
<ul><li><code>MAX_VERSIONS &lt;n&gt;</code>
</li><li><code>CELL_LIMIT &lt;n&gt;</code>
</li><li><code>KEYS_ONLY</code></li></ul>

See <a href='http://www.hypertable.org/hql/select.html'>SELECT</a> documentation for details.<br>
<br>
<b>Example:</b>

<pre><code>hadoop jar /usr/lib/hadoop-0.20/contrib/streaming/hadoop-streaming-0.20.2-cdh3u0.jar \<br>
-libjars /opt/hypertable/current/lib/java/hypertable-0.9.5.0.pre4.jar,/opt/hypertable/current/lib/java/libthrift-0.6.0.jar \<br>
-Dhypertable.mapreduce.namespace=test \<br>
-Dhypertable.mapreduce.input.table=wikipedia \<br>
-Dhypertable.mapreduce.output.table=wikipedia2 \<br>
-Dhypertable.mapreduce.input.scan_spec.options="MAX_VERSIONS 1 KEYS_ONLY CELL_LIMIT 2" \<br>
-mapper /bin/cat -reducer /bin/cat \<br>
-inputformat org.hypertable.hadoop.mapred.TextTableInputFormat \<br>
-outputformat org.hypertable.hadoop.mapred.TextTableOutputFormat \<br>
-input wikipedia -output wikipedia2<br>
</code></pre>

<h3>Row Interval</h3>

To restrict the MapReduce to a specific row interval of the input table, a row range can be specified with the <code>hypertable.mapreduce.input.scan_spec.row_interval</code> Hadoop configuration property.  The row interval predicate is specified using the same format as the timestamp predicate in the <code>WHERE</code> clause of the <code>SELECT</code> statement.  For example:<br>
<br>
<ul><li><code>ROW &lt; foo</code>
</li><li><code>ROW &gt;= bar</code>
</li><li><code>bar &lt;= TIMESTAMP &lt;= 'foo;'</code></li></ul>

<b>Example</b>.<br>
<br>
<pre><code>hadoop jar /usr/lib/hadoop-0.20/contrib/streaming/hadoop-streaming-0.20.2-cdh3u0.jar \<br>
-libjars /opt/hypertable/current/lib/java/hypertable-0.9.5.0.pre4.jar,/opt/hypertable/current/lib/java/libthrift-0.6.0.jar \<br>
-Dhypertable.mapreduce.namespace=test \<br>
-Dhypertable.mapreduce.input.table=wikipedia \<br>
-Dhypertable.mapreduce.output.table=wikipedia2 \<br>
-Dhypertable.mapreduce.input.scan_spec.columns="id,title" \<br>
-Dhypertable.mapreduce.input.scan_spec.row_interval="Dog &lt;= ROW &lt;= Kitchenette" \<br>
-mapper /bin/cat -reducer /bin/cat \<br>
-inputformat org.hypertable.hadoop.mapred.TextTableInputFormat \<br>
-outputformat org.hypertable.hadoop.mapred.TextTableOutputFormat \<br>
-input wikipedia -output wikipedia2<br>
</code></pre>