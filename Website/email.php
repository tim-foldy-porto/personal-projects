<!DOCTYPE html>
<html>
<head>
  <title>Haptic Glove Waitlist</title>
</head>
<body>
  <form method="post">
    Enter your email to sign up for early access to the TeleX Haptic Glove!!<br>
    <input type="text" name="textdata"><br>
    <input type="submit" name="submit">

  </form>
</body>
</html>

<?php

if(isset($_POST['textdata']))
{
$data=$_POST['textdata'];
$fp = fopen('data.txt', 'a');
fwrite($fp, $data);
fwrite($fp, ', ');
fclose($fp);
}
?>
