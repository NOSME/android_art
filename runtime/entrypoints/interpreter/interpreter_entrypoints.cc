/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "class_linker.h"
#include "interpreter/interpreter.h"
#include "invoke_arg_array_builder.h"
#include "mirror/abstract_method-inl.h"
#include "mirror/object-inl.h"
#include "object_utils.h"
#include "runtime.h"
#include "stack.h"

namespace art {

extern "C" void artInterperterToCompiledCodeBridge(Thread* self, MethodHelper& mh,
                                                   const DexFile::CodeItem* code_item,
                                                   ShadowFrame* shadow_frame, JValue* result)
    SHARED_LOCKS_REQUIRED(Locks::mutator_lock_) {
  mirror::AbstractMethod* method = shadow_frame->GetMethod();
  // Ensure static methods are initialized.
  if (method->IsStatic()) {
    Runtime::Current()->GetClassLinker()->EnsureInitialized(method->GetDeclaringClass(), true, true);
  }
  uint16_t arg_offset = (code_item == NULL) ? 0 : code_item->registers_size_ - code_item->ins_size_;
  ArgArray arg_array(mh.GetShorty(), mh.GetShortyLength());
  arg_array.BuildArgArray(shadow_frame, arg_offset);
  method->Invoke(self, arg_array.GetArray(), arg_array.GetNumBytes(), result, mh.GetShorty()[0]);
}

}  // namespace art
