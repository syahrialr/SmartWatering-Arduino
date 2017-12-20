<?php
//Database Connection
$koneksi = mysqli_connect("krstudio.web.id","username","password","database")
    or die("Error ".mysqli_error($koneksi));
//Take data form table on database MySQL
$sql = "SELECT * FROM greenhouse ORDER BY id DESC LIMIT 30";
$result = mysqli_query($koneksi, $sql)
    or die("Error in Selecting " . mysqli_error($koneksi));
//Make array
$greenh = array();
while($row =mysqli_fetch_assoc($result))
{
  $greenh[] = $row;
}
//Display convert data form table identity to format JSON
echo json_encode($greenh);
//close the db connection
mysqli_close($koneksi);
//refresh 11 second
?>
<meta http-equiv="refresh" content="11">