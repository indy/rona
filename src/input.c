bool key_down(RonaInput *input, RonaKey key) {
  return input->key[input->idx][key] == ButtonState_Down;
}
bool key_up(RonaInput *input, RonaKey key) {
  return input->key[input->idx][key] == ButtonState_Up;
}
bool key_pressed(RonaInput *input, RonaKey key) {
  return input->key[input->idx][key] == ButtonState_Down && input->key[1 - input->idx][key] == ButtonState_Up;
}
