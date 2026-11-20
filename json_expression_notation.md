Last week I wrote a new language required for automating some tasks for my job. It's a programming language for picking up values from JSON docs, and works as follows:

Given the following json:

```json
[
        { "properties" : [{ "name" : "color", "type" : "string"}, { "name" : "age", "type" : "int"} ]
        , "selection"  : {"index" : 1 , "active" : true }
        , "people"     :
                [{ "name"     : "David"
                 , "property" :
                        { "color" : "red", "age" : 25 }
                },
                { "name"      : "Charles"
                 , "property" :
                        { "color" : "green", "age" : 32 }
                },
                { "name"      : "Charles"
                 , "property" :
                        { "color" : "blue", "age" : 95 }
                },
                ],
        },
]
```

An expression in this language (which I've called JSON expression notation), resolves as follows:

Expression:

```json
{people[2].name}'s {properties[selection.index].name}: {people[2].property.{properties[selection.index].name}}

```
Resolution:

---------------- Step 1:

```json
{[{ "name"     : "David"
  , "property" :
         { "color" : "red", "age" : 25 }
 },
  { "name"     : "Charles"
  , "property" :
         { "color" : "green", "age" : 32 }
 },
  { "name"     : "Charles"
  , "property" :
         { "color" : "blue", "age" : 95 }
 },
][2].name}'s {[{ "name" : "color", "type" : "string"}, { "name" : "age", "type" : "int"} ][selection.index].name}: {[{ "name"     : "David"
  , "property" :
         { "color" : "red", "age" : 25 }
 },
  { "name"     : "Charles"
  , "property" :
         { "color" : "green", "age" : 32 }
 },
  { "name"     : "Charles"
  , "property" :
         { "color" : "blue", "age" : 95 }
 },
][2].property.{properties[selection.index].name}}

```
---------------- Step 2:

```json
{{ "name"     : "Charles"
  , "property" :
         { "color" : "blue", "age" : 95 }
 }.name}'s {[{ "name" : "color", "type" : "string"}, { "name" : "age", "type" : "int"} ][1].name}: {{ "name"     : "Charles"
  , "property" :
         { "color" : "blue", "age" : 95 }
 }.property.{properties[selection.index].name}}

```
---------------- Step 3:

```json
{"Charles"}'s {{ "name" : "age", "type" : "int"}.name}: {{ "color" : "blue", "age" : 95 }.{properties[selection.index].name}}

```
---------------- Step 4:

```json
Charles's {"age"}: {{ "color" : "blue", "age" : 95 }.{[{ "name" : "color", "type" : "string"}, { "name" : "age", "type" : "int"} ][selection.index].name}}

```
---------------- Step 5:

```json
Charles's age: {{ "color" : "blue", "age" : 95 }.{[{ "name" : "color", "type" : "string"}, { "name" : "age", "type" : "int"} ][1].name}}

```
---------------- Step 6:

```json
Charles's age: {{ "color" : "blue", "age" : 95 }.{{ "name" : "age", "type" : "int"}.name}}

```
---------------- Step 7:

```json
Charles's age: {{ "color" : "blue", "age" : 95 }.{"age"}}

```
---------------- Step 8:

```json
Charles's age: {{ "color" : "blue", "age" : 95 }.age}

```
---------------- Step 9:

```json
Charles's age: {95}

```
---------------- Step 10:

```json
Charles's age: 95
