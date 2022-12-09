<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1"/>
    <title>Buzzer app</title>
    <style>
        div.wrapper {
            width: 300px;
            height:300px;
            border:1px solid black;
        }

        input {
             display: block;
             margin : 0 auto;
        }
    </style>
</head>
<body>
    <div class="center">
        <form method="post">
            <input type="number" pattern="\d*" name="textdata" style="width: 80%; height: 250px; font-size: 16px" autofocus><br>
            <input type="submit" name="submit" style="width: 80%; height: 100px">
        </form>
    </div>
</body>
</html>

<?php

if(isset($_POST['textdata']))
  {
    $data=$_POST['textdata'];
    $fp = fopen('data2.txt', 'w');
    fwrite($fp, $data.'0'.time());
    fclose($fp);
  }
?>
