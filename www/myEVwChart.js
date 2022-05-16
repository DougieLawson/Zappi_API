/*
   Copyright © Dougie Lawson, 2020-2022, All rights reserved.
 */

function renderChart(dt, fu, c, h, ex) {

	var ctx = document.getElementById("myChart").getContext('2d');
	var myChart = new Chart(ctx, {
		type: 'line',
		data: {
		labels: dt,
		datasets: [
			{
			label: 'Full Usage',
			data: fu,
			yAxisID: 'Left',
			borderColor: 'rgba(75, 192, 192, 1)',
			backgroundColor: 'rgba(75, 192, 192, 0.2)',
			},
			{
			label: 'House',
			data: h,
			yAxisID: 'Left',
			borderColor: 'rgba(251, 115, 115, 1)',
			backgroundColor: 'rgba(251, 115, 115, 0.2)',
			},
			{
			label: 'Charging',
			data: c,
			yAxisID: 'Right',
			borderColor: 'rgba(163, 73, 0, 1)',
			backgroundColor: 'rgba(163, 73, 0, 0.2)',
			},
			{
			label: 'Exporting',
			data: ex,
			yAxisID: 'Right',
			borderColor: 'rgba(0, 73, 163, 1)',
			backgroundColor: 'rgba(0, 73, 163, 0.2)',
			},

		]
	},
	options: {
		spanGaps: true,
		scales: {
			xAxes: [{
				time: {
					unit: 'minute',
					displayFormats: {
						minute: 'DD HH:MM',
					},
				},
			}],
			yAxes: [{
				id: 'Left',
				type: 'linear',
				position: 'left',
				ticks: {
					max: 16500
				}
			},
			{
				id: 'Right',
				type: 'linear', 
				position: 'right',
				ticks: {
					max: 16500
				}
			}]
			},
		}
	});
}

function getChartData() {
	$("#loadingMessage").html('<img src="spinner.gif" alt="" srcset="">');
	$.getJSON("../cgi-bin/weeklyEV",
	function (data) {
		$("#loadingMessage").html("");
		var dt = [];
		var fu = [];
		var c = [];
		var h = [];
		var ex = [];
		data.forEach(function(item) {
			dt.push(item['date_time']);
			fu.push(item['Full_usage']);
			c.push(item['Charging']);
			h.push(item['House']);
			ex.push(item['Exported']);
		});
		renderChart(dt, fu, c, h, ex);
		});
	};
