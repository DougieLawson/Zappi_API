## Simple JSON Graphing example

This is a simple example of what can be done once you've got the data in a local MySQL/MariaDB table. It's using the [https://chartjs.org](https://chartjs.org) library as that's an easy way to build a HTML5 chart.

This is missing the JQuery and ChartJS code as it's better to get the latest public versions of those Javascript libraries. Run ./install.sh to pull the latest versions. 

Add a spinner GIF if the Chart JSON takes a long time loading. See
    
	$("#loadingMessage").html('<img src="spinner.gif" alt="" srcset="">');

in the Javascript code.
