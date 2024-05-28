# Track user activity
The problem is solved using Map-Reduce technique.

**Problem:** find minimum, maximum and average time each user spends on each website.

- **Input data:** file with strings, each includes website name, user id and time spent by the user on the website during one session.  
String format: <website_name> \t <user_id> # \<time\>

- **Output data:** file with strings that contain the answer.  
String format: <user_id> \t <website_name1> $ <min_time1> $ <max_time1> $ <avg_time1> # <website_name2> $ <min_time2> $ <max_time2> $ <avg_time2> # ...
