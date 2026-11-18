<?php
/*
 **************************************************************************
 *
 *
 *  (c) 2012 Copyright Spectrum Bridge, Inc.
 *  All rights reserved.
 *
 *  This software contains confidential and proprietary information.
 *  The content or any derivative work may be used only as authorized
 *  by a licensing agreement from Spectrum Bridge, Inc.  All copies must
 *  include this header.
 *
 *
 **************************************************************************
*/

/*      upgrade.php - $Revision: 96 $ $Date: 2012-10-25 14:30:49 -0400 (Thu, 25 Oct 2012) $ $Author: mario $
 *	This script allows for upgrading the UI and whitespaceagent
 *	
 */
 
  // Location of the file once uploaded
  $target_directory = "/disk/";
  
  // Full Path
  $TGZ_FILE = $target_directory . $_FILES['uploadedfile']['name'];
  $UPGRADE_FILE = $target_directory . "meldtech/upgrade.sh";

  include('processor.php');

  if(isset($_POST['submit'])) {
    //echo $_FILES['uploadedfile']['error'];
    //echo $_FILES['uploadedfile']['name']."<br>";
    //echo $_FILES['uploadedfile']['type'];
    //echo $_FILES['uploadedfile']['size'];
    //echo $_FILES['uploadedfile']['tmp_name'];
    
    $FILE_NAME = $_FILES['uploadedfile']['name'];
    // CHECK TYPE TO BE COMPRESSED TAR AND THAT SIZE IS NOT MORE THAN 8M
    if ((end(explode('.',$_FILES['uploadedfile']['name'])) == "tgz") && ($_FILES['uploadedfile']['size'] < 8000000)) {
      if ($_FILES['uploadedfile']['error'] > 0) {
        echo "There was an error processing the upload. Error code: " . $_FILES['uploadedfile']['error'];
      } else {
        move_uploaded_file($_FILES['uploadedfile']['tmp_name'],$TGZ_FILE);
        //echo "Stored in: " . $TGZ_FILE;
        if (file_exists($TGZ_FILE)) {
          $UPLOAD_SUCCESS = TRUE;
        }
      }
    } else {
      echo "<script type='text/javascript'>
	window.alert('The selected file is not the proper format!')
	</script>";
    }
    
    sleep(2);
    // DECOMPRESS FILE
    if ($UPLOAD_SUCCESS) {
      //$toexec = "decompress " . $FILE_NAME; 
      $toexec = "tar -xzf $TGZ_FILE -C $target_directory";
      exec("$toexec &> upgrade.log");
      if (file_exists($UPGRADE_FILE)) {
        exec("$UPGRADE_FILE &> upgrade.log",$output,$return);
        if (!$return) {
          exec("rm -Rf $target_directory/meldtech"); 
          exec("rm $TGZ_FILE");
          $SUCCESS = true;
        }
      }
    }
  } else if (isset($_REQUEST['reset'])) {
    if (clearSbiFile()) {
      if (resetSetipeth0()) {
	$SUCCESS = true;
      }
    }
  } // END reset post
?>
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" xmlns:xml="http://www.w3.org/XML/1998/namespace" lang="en">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<meta http-equiv="Content-Script-Type" content="text/javascript" />
<link rel="stylesheet" type="text/css" media="screen" href="resources/cascade.css" />
<script type="text/javascript" src="resources/xhr.js"></script>
<title>MELDTECH - Advanced - Tools</title>
</head>
<body class="lang_en">

<?php include 'header.php'; ?>

<div id="maincontainer">
        <div id="tabmenu">

        <br style="clear:both" />

        <div class="tabmenu1">
        <ul class="tabmenu l1">

                        <li class="tabmenu-item-registration">
                                <a href="registration.php">White Space Registration</a>

                        </li>

                        <li class="tabmenu-item-achannels">
                                <a href="status.php">Status</a>

                        </li>

                        <li class="tabmenu-item-advanced active">
                                <a href="preferences.php">Advanced</a>

                        </li>

        </ul>
        <br style="clear:both" />
        
	<div class="tabmenu2">
	<ul class="tabmenu l2">
	<li class="tabmenu-item-preferences">
		<a href="preferences.php">Preferences</a>
	</li>

	<li class="tabmenu-item-network">
                <a href="network.php">Network</a>
        </li>

	<li class="tabmenu-item-achannels active">
		<a href="upgrade.php">Tools</a>
	</li>
	</ul>
																													<br style="clear:both" />
	</div>
        </div>

        </div>


	<div id="maincontent">
<form method="post" name="cbi" action="<?php echo htmlentities($_SERVER['PHP_SELF']); ?>" enctype="multipart/form-data" onsubmit="return confirm('Are you sure you want to continue?\nNOTE: The device might need to be rebooted for changes to take effect!')">
        <div>
                <script type="text/javascript" src="resources/cbi.js"></script>
                <input type="hidden" name="cbi.submit" value="1" />
                <input type="submit" value="Save" class="hidden" />
        </div>


<div class="cbi-map" id="cbi-channels">
<?php
  if(isset($_POST['submit'])) {
    if ($SUCCESS) {
    echo "<font color='red'>The softgrade upgrade has been successfully applied!</font><br />Please reboot the device for changes to take effect!";
    } else {
    echo "<font color='red'>The softgrade upgrade was NOT successfully applied!</font><br />Please try again!";
    }
  } else if (isset($_POST['reset'])) {
    if ($SUCCESS) {
      echo "<font color='red'>The device has been SUCCESSFULLY reset to factory defaults!</font><br />Please reboot the device for changes to take effect!";
    } else {
      echo "<font color='red'>There was a problem resetting the device to factory defaults. Please try again!</font>";
    }
  }
?>
        
<h2><a id="content" name="content">Software Tools</a></h2>
<fieldset class="cbi-section" id="cbi-preferences">
  <b>Software Upgrade</b>
  <div class="cbi-section-descr"></div>
  <fieldset class="cbi-section-node" id="cbi-sbi-meldtech">

<!-- PICK THE FILE -->
    <div class="cbi-value" id="cbi-sbi-meldtech-upgrade"><label class="cbi-value-title" for="cbid.sbi.meldtech.Upgrade">Select Upgrade File</label>
      <div class="cbi-value-field">
	<input type="file" name="uploadedfile" id="file" />
      </div>
    </div>
 </fieldset>
    <div class="cbi-page-actions">
      <input class="cbi-button cbi-button-apply" type="submit" name="submit" value="Save &amp; Apply" />
      <script type="text/javascript">cbi_d_update();</script>
    </div>
   <br />
</fieldset>

<!-- Reset --> 
<fieldset class="cbi-section" id="cbi-preferences">
  <b>Factory Reset</b>
  <div class="cbi-section-descr"></div>
  <fieldset class="cbi-section-node" id="cbi-sbi-meldtech">
    <div class="cbi-value" id="cbi-sbi-meldtech-reset"><label class="cbi-value-title" for="cbid.sbi.meldtech.Reset">Reset to Factory Defaults</label>
      <div class="cbi-value-field">
        &nbsp; 
      </div>
    </div>
  </fieldset>
    <div class="cbi-page-actions">
      <input type="submit" class="cbi-button cbi-button-apply" name="reset" value="Reset" /> 
    </div>
  <br />
</fieldset>

</div>
</form>
<h3>© 2012 Spectrum Bridge, Inc. All Rights Reserved.</h3>
</div>
</div>

</body>
</html>
