

# Introduction #

This document describes how a simple Twitter-like application can be built on Hypertable.  It's a stripped down version of Twitter that contains a user's tweet stream as well as current user information (number of tweets, latest tweet, following, and followers).

# Details #

## Tables ##

### Table: `profile` ###

_rowkey:  username_

```
CREATE TABLE profile (
  guid,
  'display_name',
  password,
  avatar,
  bio,
  url
);
```

This table contains profile information for each user.

### Table: `user` ###

_rowkey: username_

```
CREATE TABLE user (
  following MAX_VERSIONS=1,
  'following_history',
  'following_count' COUNTER,
  followers MAX_VERSIONS=1,
  'followers_history',
  'followers_count' COUNTER,
  'follow_stream',
  'follow_stream_count' COUNTER,
  'my_stream',
  'my_stream_count' COUNTER
);
```

### Table: `tweet` ###

_rowkey: username+timestamp\_milliseconds_

```
CREATE TABLE tweet (
  message
);
```

## Actions ##

### Register New User ###

The first step of this action is to find a unique username.  This will be accomplished by calling an API (_TBD_) that allows you to atomically insert a column, if it does not already exists, and return the column value.  For example:

```
$username = /* ask user for username */
$guid = /* generate guid */
$old_guid = 0
while ($guid != $old_guid) {
  if ($old_guid != 0) {
    /* report error; ask user for a different username */
  }
  $old_guid = INSERT_ONCE_RETURN INTO profile ($username, "guid", "$guid")
}
```

Once a unique username (row in the profile table) has been selected, the user's profile information can be inserted into to the profile table.

### Follow ###

Let's say user "joe" decides to follow user "bob".  The first step is to insert into the `following` and `followers` columns of the respective user rows:

```
INSERT INTO user VALUES ("joe", "following:bob", ""), ("bob", "followers:joe", "");
```

Then, obtain the timestamps of the cells that were just inserted and use those timestamps to insert entries into the `following_history` and `followers_history` columns and increment the `following_count` and `followers_count` counters.

```
SELECT CELLS * FROM user WHERE CELL = "joe","following:bob" OR CELL = "bob","followers:joe";
$following_timestamp = /* set to joe following:bob cell timestamp */
$followers_timestamp = /* set to bob followers:joe cell timestamp */
INSERT INTO user VALUES ($following_timestamp, "joe", "following_history", "bob"), ($followers_timestamp, "bob", "followers_history", "joe"), ("joe", "following_count", "+1"), ("bob", "followers_count", "+1");
```

### Unfollow ###

This involves undoing all of the user table updates that were performed during the `follow` action.

```
SELECT CELLS * FROM user WHERE CELL = "joe","following:bob" OR CELL = "bob","followers:joe";
$following_timestamp = /* set to joe following:bob cell timestamp */
$followers_timestamp = /* set to bob followers:joe cell timestamp */
DELETE "following_history" WHERE ROW = "joe" VERSION $following_timestamp;
DELETE "followers_history" WHERE ROW = "bob" VERSION $followers_timestamp;
DELETE "following:bob" WHERE ROW = "joe";
DELETE "followers:joe" WHERE ROW = "bob"
INSERT INTO user VALUES ("joe", "following_count", "-1"), ("bob", "followers_count", "-1");
```

### Tweet ###

Generate a unique "tweetid" for the tweet by combining the username plus a high resolution timestamp.  Then perform the following table updates:

  * Insert the tweet into the tweet table
  * Add to the user's my\_stream column in the users table with the new tweet id
  * For each of the users followers, add the tweet id to their follow\_stream

```
$message = /* user's tweet */
$tweetid = /* username + timestamp */
INSERT INTO tweet VALUES ("$tweetid", "message", "$message");
INSERT INTO user VALUES ("$username", "my_stream", "$tweetid"), ("$username", "my_stream_count", "+1");
SELECT follow_stream FROM user WHERE ROW = "$username");
foreach (column_qualifier as $qualifier) {
  INSERT INTO user VALUES ("$qualifier", "follow_stream", "$tweetid"), ("$qualifier", "follow_stream_count", "+1")
}
```

## Rendering ##

### User Information ###

```
/* latest tweet */
SELECT my_stream FROM user WHERE ROW=$username CELL_LIMIT 1;

/* tweet count */
SELECT my_stream_count FROM user WHERE ROW=$username;

/* following count */
SELECT following_count FROM user WHERE ROW=$username;

/* most recent 5 people user is following */
SELECT following_history FROM user WHERE ROW=$username CELL_LIMIT 5; 
foreach (value_in_previous_select_command as $person) {
  select avatar from profile where ROW=$person;
  /* display avatar */
}

/* followers count */
SELECT followers_count FROM user WHERE ROW=$username;

/* most recent 5 people who are following user */
SELECT followers_history FROM user WHERE ROW=$username CELL_LIMIT 5; 
foreach (value_in_previous_select_command as $person) {
  select avatar from profile where ROW=$person;
  /* display avatar */
}
```

### Tweet Stream ###

```
/* New Tweet count */
$newcount = SELECT follow_stream_count FROM user WHERE ROW=$username;
$newtweets = $newcount-$oldcount

/* Tweet stream - first 20 tweets */
SELECT follow_stream FROM user WHERE ROW=$username CELL_LIMIT 20;
$cutoff_timestamp = /* timestamp of last cell in above select statement */

/* Tweet stream - next 20 tweets */
SELECT follow_stream FROM user WHERE ROW=$username AND TIMESTAMP < $cutoff_timestamp CELL_LIMIT 20;
$cutoff_timestamp = /* timestamp of last cell in above select statement */
```

### Other Users's Recent Tweets ###

```
/* To obtain the latest 3 tweets of $user */
SELECT my_stream FROM user WHERE ROW=$user CELL_LIMIT 3;
$cutoff_timestamp = /* timestamp of last cell in above select statement */

/* To obtain the next latest 20 tweets of $user */
SELECT my_stream FROM user WHERE ROW=$user AND TIMESTAMP < $cutoff_timestamp CELL_LIMIT 20;
$cutoff_timestamp = /* timestamp of last cell in above select statement */
```