<!DOCTYPE html>
<html>
<head>
  <title>Haptic Glove Waitlist</title>
</head>
<body>
  <form method="post">
    <input type="text" name="textdata"><br>
    <input type="submit" name="submit">

  </form>
</body>
</html>

<?php

if(isset($_POST['textdata']))
  {
    $data=$_POST['textdata'];
    $fp = fopen('data2.txt', 'w');
    fwrite($fp, $data);
    fclose($fp);
  }
?>
