//Default Values
var xAxisLabel;
var yAxisLabel;
var whichXMetric = "time";
var whichYMetric = "current";
var svg;

var margin = {top: 20, right: 20, bottom: 30, left: 50},
	width = 960 - margin.left - margin.right,
	height = 500 - margin.top - margin.bottom;


format = d3.time.format("%X").parse;
var xVals = d3.time.scale().range([0, width]);
var yVals = d3.scale.linear().range([height, 0]);
var xAxis = d3.svg.axis().scale(xVals).orient("bottom").ticks(18);
var yAxis = d3.svg.axis().scale(yVals).orient("left");

var lineGraph = d3.svg.line()
	.x(function (d){ return xVals(d.xMetric); })
	.y(function (d){ return yVals(d.yMetric); });

		
var produceGraph = function() { d3.csv("test_dataset.csv", function(data, error) {
  data.forEach( function(d) {
		if (whichXMetric == "time")
			d.xMetric = format(d.Time);
		else if (whichXMetric == "current")
			d.xMetric = +d.Current;
		else if (whichXMetric == "voltage")
			d.xMetric = +d.Voltage;
		else if (whichXMetric == "power")
			d.xMetric = +d.Power;
		else if (whichXMetric == "temperature")
			d.xMetric = +d.Temperature;
		else if (whichXMetric == "luminosity")
			d.xMetric = +d.Luminosity;
			
		if (whichYMetric == "time")
			d.yMetric = format(d.Time);
		else if (whichYMetric == "current")
			d.yMetric = +d.Current;
		else if (whichYMetric == "voltage")
			d.yMetric = +d.Voltage;
		else if (whichYMetric == "power")
			d.yMetric = +d.Power;
		else if (whichYMetric == "temperature")
			d.yMetric = +d.Temperature;
		else if (whichYMetric == "luminosity")
			d.yMetric = +d.Luminosity;
	});

  svg = d3.select("body").append("svg")
	.attr("width", width + margin.left + margin.right)
	.attr("height",height + margin.top + margin.bottom)
  .append("g")
	.attr("transform", "translate(" + margin.left + "," + margin.top + ")");
  
  xVals.domain( d3.extent(data, function(d) { return d.xMetric; } ));
  yVals.domain( d3.extent(data, function(d) { return d.yMetric; } ));
  
  svg.append("g")
	  .attr("class", "x axis")
	  .attr("transform", "translate(0, " + height + ")")
	  .call(xAxis)
	.append("text")
	  .attr("x", 10)
	  .attr("dx", width - margin.right)
	  .style("text-anchor", "end")
	  .text(xAxisLabel);
	  
  svg.append("g")
	  .attr("class", "y axis")
	  .call(yAxis)
	.append("text")
	  .attr("transform", "rotate(-90)")
	  .attr("y", 5)
	  .attr("dy", ".71em")
	  .style("text-anchor", "end")
	  .text(yAxisLabel);

  svg.append("path")
	  .datum(data)
	  .attr("class", "lineGraph")
	  .attr("d", lineGraph);		
	}); 
}
//genericVar could be an axisLabel or d.yMetric or d.xMetric
function assignMetrics(whichMetric, genericVar, typeOfVar){
	switch (whichMetric){
		case "time":
			if (typeOfVar == "axis")
				genericVar = "Time (Hour:Minute)";
			break;
		case "current":
		    if (typeOfVar == "axis")
				genericVar = "Current (Amps)";
			break;
		case "voltage":
			if (typeOfVar == "axis")
				genericVar = "Voltage (Volts)";
			break;
		case "power":
			if (typeOfVar == "axis")
				genericVar = "Power (Watts)";
			break;
		case "temperature":
			if (typeOfVar == "axis")
				genericVar = "Temperature (Celsius)";
			break;
		case "luminosity":
			if (typeOfVar == "axis")
				genericVar = "Luminosity (Watts/m^2)";
			break;
	}
	return genericVar;
}