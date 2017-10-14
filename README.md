# Ares Studios

The project code and insights for the project developed by Ares Studios

## Subsections
* Phaser Control - The Arduino code powering the gun Arduino system
* RC Car - The Arduino code powering the RC Car, hit detection and damage response

## Phaser Control
System will either have shot capacity or duration of firing restrictions requiring
a reload/recharge after expended. The phaser operator will receive this feedback
via an LCD digit display on the phaser body. When the trigger is pulled, the phaser
will engage the laser. With the current design, the phaser doesn't need to be aware of
the success of the shot. It will deliver audio and potentially haptic feedback to the user
with each phaser shot.

## RC Car
Generalized system supports control over Bluetooth using the Blynk app.
It accepts input from hit detection sensor arrays on set locations around chassis
and logs damage to various areas indepdently. If one area goes above the threshold, the
car will disable and the user has won.

## Contribution Guidelines
Code should be properly placed in appropriate directories and
new features should be added as issues to keep an easy log of
ongoing development efforts.
