<html>
<head>
<title>PipeTest</title>
</head>
<body>
<?php
	$hFile=@fopen("\\\\.\\pipe\\phpPipeDatenbank","w+r+b");
	@fwrite($hFile,"TestFrage");
	$Result=@fread($hFile,64);
	echo $Result;
	@fclose($hFile);
?>
</body>
</html>