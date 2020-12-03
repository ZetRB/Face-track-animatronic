void bothBrows(int movement) {
  leftBrow.writeMicroseconds(1720 + movement);
  rightBrow.writeMicroseconds(950 - movement);
}

void leftBrowMove(int movement) {
  leftBrow.writeMicroseconds(1720 + movement);
}

void rightBrowMove(int movement) {
  rightBrow.writeMicroseconds(950 - movement);
}


void lookUpAndFrown() {
  if (millis() - randomStart < 1000) {
    upDown.writeMicroseconds(yCentre);
    bothBrows(-250);
  } else if (millis() - randomStart < intervalA) {
    leftRight.writeMicroseconds(xCentre + 250);
  } else if (millis() - randomStart < intervalB) {
    leftRight.writeMicroseconds(xCentre - 250);
  } else if (millis() - randomStart < intervalC) {
    leftRight.writeMicroseconds(xCentre + 250);
  } else if (millis() - randomStart > intervalD) {
    resetRandom();
  }
}

void eyebrowMove(int option) {
  if (option == 1) {
    eyebrowRaise(lr);
  } else if (option == 2) {
    eyebrowFlex();
  }
}

void eyebrowRaise(int side) {
  if (millis() > noMoreData) {
    readBook();
  }
  if (millis() - randomStart < 2 * intervalA) {
    if (side == 0) {
      leftBrowMove(0);
      rightBrowMove(-300);
    } else {
      leftBrowMove(-300);
      rightBrowMove(0);
    }
  } else {
    resetRandom();
  }
}

void eyebrowFlex() {
  if (millis() - randomStart < 200) {
    bothBrows(0);
  } else if (millis() - randomStart < 600) {
    bothBrows(-250);
  } else if (millis() - randomStart < 800) {
    bothBrows(0);
  } else if (millis() - randomStart < 900) {
    bothBrows(-250);
  } else if (millis() - randomStart > 900) {
    resetRandom();
  }
}
