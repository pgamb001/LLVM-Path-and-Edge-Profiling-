; ModuleID = 'support/2.bc'
target datalayout = "e-i64:64-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@bbCounter = internal global i32 0
@BasicBlockPrintfFormatStr = private constant [14 x i8] c"BB Count: %d\0A\00"
@EdgeProfCounters = internal global [12 x i64] zeroinitializer
@PathProfCounters = internal global [12 x i64] zeroinitializer

; Function Attrs: nounwind uwtable
define void @function_1(i32 %x) #0 {
b:
  %0 = load i32* @bbCounter
  %1 = add i32 1, %0
  store i32 %1, i32* @bbCounter
  %2 = alloca i32, align 4
  %y = alloca i32, align 4
  store i32 %x, i32* %2, align 4
  %3 = load i32* %2, align 4
  store i32 %3, i32* %y, align 4
  br label %b1

b1:                                               ; preds = %b3, %b
  %4 = load i32* @bbCounter
  %5 = add i32 1, %4
  store i32 %5, i32* @bbCounter
  %6 = load i32* %2, align 4
  %7 = icmp ugt i32 %6, 0
  br i1 %7, label %b3, label %b2

b2:                                               ; preds = %b1
  %8 = load i32* @bbCounter
  %9 = add i32 1, %8
  store i32 %9, i32* @bbCounter
  br label %b4

b3:                                               ; preds = %b1
  %10 = load i32* @bbCounter
  %11 = add i32 1, %10
  store i32 %11, i32* @bbCounter
  %12 = load i32* %2, align 4
  %13 = add i32 %12, -1
  store i32 %13, i32* %2, align 4
  br label %b1

b4:                                               ; preds = %b2
  %14 = load i32* @bbCounter
  %15 = add i32 1, %14
  store i32 %15, i32* @bbCounter
  %16 = load i32* %y, align 4
  store i32 %16, i32* %2, align 4
  br label %b5

b5:                                               ; preds = %b7, %b4
  %17 = load i32* @bbCounter
  %18 = add i32 1, %17
  store i32 %18, i32* @bbCounter
  %19 = load i32* %2, align 4
  %20 = icmp ugt i32 %19, 0
  br i1 %20, label %b7, label %b6

b6:                                               ; preds = %b5
  %21 = load i32* @bbCounter
  %22 = add i32 1, %21
  store i32 %22, i32* @bbCounter
  br label %b8

b7:                                               ; preds = %b5
  %23 = load i32* @bbCounter
  %24 = add i32 1, %23
  store i32 %24, i32* @bbCounter
  %25 = load i32* %2, align 4
  %26 = add i32 %25, -1
  store i32 %26, i32* %2, align 4
  br label %b5

b8:                                               ; preds = %b6
  %27 = load i32* @bbCounter
  %28 = add i32 1, %27
  store i32 %28, i32* @bbCounter
  ret void
}

; Function Attrs: nounwind uwtable
define i32 @main() #0 {
b:
  %0 = load i32* @bbCounter
  %1 = add i32 1, %0
  store i32 %1, i32* @bbCounter
  %2 = alloca i32, align 4
  store i32 0, i32* %2
  call void @function_1(i32 100)
  %3 = load i32* @bbCounter
  %4 = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([14 x i8]* @BasicBlockPrintfFormatStr, i32 0, i32 0), i32 %3)
  ret i32 0
}

declare i32 @printf(i8*, ...)

attributes #0 = { nounwind uwtable "less-precise-fpmad"="false" "no-frame-pointer-elim"="true" "no-frame-pointer-elim-non-leaf" "no-infs-fp-math"="false" "no-nans-fp-math"="false" "stack-protector-buffer-size"="8" "unsafe-fp-math"="false" "use-soft-float"="false" }

!llvm.ident = !{!0}

!0 = !{!"Ubuntu clang version 3.4-1ubuntu3 (tags/RELEASE_34/final) (based on LLVM 3.4)"}
