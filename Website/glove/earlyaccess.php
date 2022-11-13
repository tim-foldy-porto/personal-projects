<!DOCTYPE html>
<html>

  <head>
    <title>TeleX Glove</title>
    <style>
      * {
        box-sizing: border-box;
      }

      html {
        background-color: lightblue;
      }
      center {
        margin-top: 50px;
        text-align: center;
        color: yellow;
        font-size: 500%;
        font-family: "Helvetica";
        background-color: pink;
      }
      center:hover {
        /* transform: rotate(-180deg); */
        color: powderblue;
      }

      b {
        font-family: "Helvetica";
      }
      b:hover {
        color: yellow;
      }

      back {
        float: left;
        margin-left: 30px;
        font-size: 300%;
        padding: 20px;
        text-decoration: underline;
        color: black;
        background-color: orange;
      }
      back:hover {
        transform: rotate(350deg);
      }
      .text {
        text-align: center;
        padding: 40px;
        height: 10%;
        width: 70%;
        margin-left: 10%;
        font-family: "Helvetica";
        font-size: 140%;
        margin-right: 100px;
        margin-top: 15%;
        margin-bottom: 100px;
        background-color: white;
      }
      vid {
        padding: 40px;
        position: absolute;
        margin-left: 60%;
        margin-top: 100px;
      }

    </style>
  </head>

  <body>

  <div id="welcome">
    <center>EARLY ACCESS</center>
    <a href="./glove.html"><back> &larr; Back </back></a>


    <form method="post">
      Enter your email to sign up for early access to the TeleX Haptic Glove!<br>
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
