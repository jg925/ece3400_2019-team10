int navigate() {
  left_sensor_value = analogRead(LEFT_LINE_SENSOR);
  right_sensor_value = analogRead(RIGHT_LINE_SENSOR);

  // if both sensors on white
  if (left_sensor_value < line_threshold && right_sensor_value < line_threshold) {
    moveForward();
    delay(500);
    return 1; // return 1 here, else we return 0 to keep navigating
  }

  // left on white, right on black --> slight left adjust
  else if (left_sensor_value < line_threshold && right_sensor_value >= line_threshold) {
    // left detects white, right detects black
    // Serial.println("LEFT ON WHITE");
    while (left_sensor_value < line_threshold && right_sensor_value >= line_threshold) {
      slightLeft();
      left_sensor_value = analogRead(LEFT_LINE_SENSOR);
      right_sensor_value = analogRead(RIGHT_LINE_SENSOR);
    }
  }

  // right on white, left on black --> slight right adjust
  else if (left_sensor_value >= line_threshold && right_sensor_value < line_threshold) {
    // left detects black, right detects white
    //Serial.println("RIGHT ON WHITE");
    while (right_sensor_value < line_threshold && left_sensor_value >= line_threshold ) {
      slightRight();
      left_sensor_value = analogRead(LEFT_LINE_SENSOR);
      right_sensor_value = analogRead(RIGHT_LINE_SENSOR);
    }
  }

  // both sensors detect black --> go straight
  else if (left_sensor_value >= line_threshold && right_sensor_value >= line_threshold) {
    // Serial.println("ALL BLACK");
    moveForward();
  }
  return 0;
}
