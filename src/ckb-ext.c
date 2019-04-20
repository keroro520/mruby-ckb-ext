#include "mruby.h"
#include "mruby/string.h"
#include "mruby/compile.h"

#include "ckb_consts.h"
#include "protocol_reader.h"

#undef ns
#define ns(x) FLATBUFFERS_WRAP_NAMESPACE(Ckb_Protocol, x)

extern int ckb_load_tx(void* addr, uint64_t* len, size_t offset);
extern int ckb_load_cell(void* addr, uint64_t* len, size_t offset,
    size_t index, size_t source);
extern int ckb_load_cell_by_field(void* addr, uint64_t* len, size_t offset,
    size_t index, size_t source, size_t field);
extern int ckb_load_input_by_field(void* addr, uint64_t* len, size_t offset,
    size_t index, size_t source, size_t field);
extern int ckb_debug(const char* message);

static mrb_value
ckb_mrb_eval(mrb_state *mrb, mrb_value self)
{
  mrb_int index, source;
  mrb_get_args(mrb, "ii", &index, &source);

  uint64_t len = 0;
  if (ckb_load_cell_by_field(NULL, &len, 0, index, source, CKB_CELL_FIELD_DATA) != CKB_SUCCESS) {
    mrb_raise(mrb, E_ARGUMENT_ERROR, "missing target cell");
  }

  void* addr = malloc(len);
  if (addr == NULL) {
    free(addr);
    mrb_raise(mrb, E_ARGUMENT_ERROR, "not enough memory");
  }
  if (ckb_load_cell_by_field(addr, &len, 0, index, source, CKB_CELL_FIELD_DATA) != CKB_SUCCESS) {
    free(addr);
    mrb_raise(mrb, E_ARGUMENT_ERROR, "loading cell data");
  }

  mrb_value v = mrb_load_string(mrb, addr);
  free(addr);
  return v;
}

void
mrb_mruby_ckb_ext_gem_init(mrb_state* mrb)
{
  struct RClass *class__;
  class__ = mrb_define_module(mrb, "CKB");
  mrb_define_module_function(mrb, class__, "eval", ckb_mrb_eval, MRB_ARGS_REQ(2));
}

void mrb_mruby_ckb_ext_gem_final(mrb_state* mrb)
{
}
