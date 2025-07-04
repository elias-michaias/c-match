#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "../match.h"

// Test the new all-in-one tag_union macro

// Single macro call generates everything: struct, enums, constructors
tag_union(Either, int, Number, char*, Text)

tag_union(Color, int, Red, int, Green, int, Blue)

tag_union(Shape, int, Circle, float, Square, char*, Triangle, double, Rectangle)

tag_union(Value, int, IntData, char*, StrData)

// Test the tag_union macro with different arities
int main() {
    printf("Testing tag_union macro...\n");
    
    Either e1 = new_Either_Number(42);
    Either e2 = new_Either_Text("hello world");
    
    printf("Test 1: 2-variant tagged union\n");
    printf("e1.tag = %u, e1.Number = %d\n", e1.tag, e1.Number);
    printf("e2.tag = %u, e2.Text = %s\n", e2.tag, e2.Text);
    
    // Test pattern matching with tagged unions
    match(&e1) {
        when(Either_Number) {
            printf("Matched Number: %d\n", e1.Number);
        }
        when(Either_Text) {
            printf("Matched Text: %s\n", e1.Text);
        }
        otherwise {
            printf("No match\n");
        }
    }
    
    match(&e2) {
        when(Either_Number) {
            printf("Matched Number: %d\n", e2.Number);
        }
        when(Either_Text) {
            printf("Matched Text: %s\n", e2.Text);
        }
        otherwise {
            printf("No match\n");
        }
    }
    
    Color c1 = new_Color_Red(255);
    Color c2 = new_Color_Green(128);
    Color c3 = new_Color_Blue(64);
    
    printf("\nTest 2: 3-variant tagged union\n");
    printf("c1.tag = %u, c1.Red = %d\n", c1.tag, c1.Red);
    printf("c2.tag = %u, c2.Green = %d\n", c2.tag, c2.Green);
    printf("c3.tag = %u, c3.Blue = %d\n", c3.tag, c3.Blue);
    
    // Test pattern matching with 3-variant union
    match(&c1) {
        when(Color_Red) {
            printf("Matched Red: %d\n", c1.Red);
        }
        when(Color_Green) {
            printf("Matched Green: %d\n", c1.Green);
        }
        when(Color_Blue) {
            printf("Matched Blue: %d\n", c1.Blue);
        }
        otherwise {
            printf("No match\n");
        }
    }
    
    Shape s1 = new_Shape_Circle(10);
    Shape s2 = new_Shape_Square(5.5f);
    Shape s3 = new_Shape_Triangle("isosceles");
    Shape s4 = new_Shape_Rectangle(3.14159);
    
    printf("\nTest 3: 4-variant tagged union\n");
    printf("s1.tag = %u, s1.Circle = %d\n", s1.tag, s1.Circle);
    printf("s2.tag = %u, s2.Square = %f\n", s2.tag, s2.Square);
    printf("s3.tag = %u, s3.Triangle = %s\n", s3.tag, s3.Triangle);
    printf("s4.tag = %u, s4.Rectangle = %f\n", s4.tag, s4.Rectangle);
    
    // Test pattern matching with 4-variant union
    match(&s1) {
        when(Shape_Circle) {
            printf("Matched Circle: %d\n", s1.Circle);
        }
        when(Shape_Square) {
            printf("Matched Square: %f\n", s1.Square);
        }
        when(Shape_Triangle) {
            printf("Matched Triangle: %s\n", s1.Triangle);
        }
        when(Shape_Rectangle) {
            printf("Matched Rectangle: %f\n", s1.Rectangle);
        }
        otherwise {
            printf("No match\n");
        }
    }
    
    // Test expression form with tagged unions
    int result1 = match_expr(&e1) in(
        is(Either_Number) ? e1.Number * 2 :
        is(Either_Text) ? strlen(e1.Text) : 0
    );
    
    int result2 = match_expr(&e2) in(
        is(Either_Number) ? e2.Number * 2 :
        is(Either_Text) ? strlen(e2.Text) : 0
    );
    
    printf("\nTest 4: Expression form with tagged unions\n");
    printf("result1 (Number * 2) = %d\n", result1);
    printf("result2 (Text length) = %d\n", result2);
    
    // Test direct tag access
    printf("\nTest 5: Direct tag access\n");
    printf("e1.tag = %u\n", e1.tag);
    printf("e1.tag == Either_Number = %d\n", e1.tag == Either_Number);
    printf("e1.tag == Either_Text = %d\n", e1.tag == Either_Text);
    
    // Test that we can use different field names with different types
    Value v1 = new_Value_IntData(123);  // This should work - using the first constructor
    printf("\nTest 6: Different field names with different types\n");
    printf("v1.tag = %u, v1.IntData (as int) = %d\n", v1.tag, v1.IntData);
    
    printf("\nAll tag_union tests passed!\n");
    return 0;
}
