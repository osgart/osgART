
/*
 * SVG based marker generator for SSTT frame ID markers
 *
 * (c) Copyrights 2007-2010 Hartmut Seichter
 *
 */
var g_on_style  = 'fill:#000;fill-opacity:1;stroke:none';
var g_off_style = 'fill:#fff;fill-opacity:1;stroke:none';
var g_ovr_style = 'fill:#abb;fill-opacity:1;stroke:none';

var g_value = 14681358;


function get_bin_name(row,bin) {
	return 'b-' + row + '-' + bin;
}

function test_bin_hover(row,bin,over) {

	var bin_name = get_bin_name(row,bin);
	var el = document.getElementById(bin_name);
	var state = el.getAttribute('data');
		
	if (over > 0)
	{
		el.setAttribute("style",g_ovr_style);
	} else {

		if (state > 0) {
			el.setAttribute("style",g_on_style);
		} else {
			el.setAttribute("style",g_off_style);
		}
	}
}


function bin_get_id_from_display() {

	sum = 0;
	count = 0;

	for (var r = 0; r < 3; r++) {
		for (var b = 0; b < 8; b++) {
			var bin_name = get_bin_name(r,b);
			var el = document.getElementById(bin_name);

			if (el.getAttribute('data') > 0) {
				
				sum += (1 << count);							
			}
			count++;
		}
	}

	return sum;

}

function bin_sync_display() {

	for (var r = 0; r < 3; r++) {
		for (var b = 0; b < 8; b++) {
			var bin_name = get_bin_name(r,b);
			if (0 == document.getElementById(bin_name).getAttribute('data')) {
				document.getElementById(bin_name).setAttribute("style",g_off_style);
			} else {
				document.getElementById(bin_name).setAttribute("style",g_on_style);
			}
		}
	}
}

function bin_sync_id() {

	document.getElementById('id-text').setAttribute("style","font-family:helvetica;font-style:bold;font-size:10pt");

	var txt = document.getElementById('id-text').firstChild.firstChild;

	txt.nodeValue = 'ID: ' + bin_get_id_from_display() + ' 0x' + bin_get_id_from_display().toString(16);
}

function bin_clicked(row,bin) {
	var bin_name = get_bin_name(row,bin);
	var el = document.getElementById(bin_name);
	var state = el.getAttribute('data');

	if (state > 0) {
		el.setAttribute('data',0);
	} else {
		el.setAttribute('data',1);
	}

	bin_sync_display();
	bin_sync_id();
}


function bin_onload() {

	count = 0;

	for (var r = 0; r < 3; r++) {
		for (var b = 0; b < 8; b++) {

			var bin_name = get_bin_name(r,b);

			var el = document.getElementById(bin_name);

			if (el) {

				el.onmouseover = new Function('test_bin_hover('+ r + ',' + b +',1);');
				el.onmouseout = new Function('test_bin_hover('+ r + ',' + b +',0);');
				el.onclick = new Function(' bin_clicked('+ r + ',' + b +');');
			
				if (g_value & (1 << count)) {
					el.setAttribute('data',1);	
				} else {
					el.setAttribute('data',0);	
				}

			} else {
				alert('Not found: ' + bin_name);
			}

			count++;
		}
	}

	bin_sync_display();
	bin_sync_id();
}


function testfunc(row,bin) {
	
	var bin_name = get_bin_name(row,bin);

	var el = document.getElementById(bin_name);
	var attr = el.getAttribute('style');

	alert('we are here! ' + bin_name + ' ' + el + ' ' + attr);
}


function on_load() {

	bin_onload();
}

window.onload = on_load;


