/* Alias for converting strings to elements */
let str = ReasonReact.stringToElement;

/* Defines an item type in the Todo list */
type item = {
  id: int,
  title: string,
  completed: bool
};

/* State of the TodoApp component is defined by how many items
   are on the Todo list */
type state = {
  items: list(item)
};

/* Actions are used to change the state of a component */
type action = 
  | AddItem(string)
  | ToggleItem(int);

/* Item that is created when the button is clicked */
let lastID = ref(0);
let newItem = (text) => {
  lastID := lastID^ + 1; /* ^ means derefencing */
  {id: lastID^, title: text, completed: false}
}; 

/* Used to get user text input somehow? */
let valueFromEvent = (evt) : string => (
  evt 
  |> ReactEventRe.Form.target
  |> ReactDOMRe.domElementToObj
)##value;

/* Defines an item in the Todo list, and action for changing
   its checkbox. This is a small component, so don't need
   new file for it can use nexted module */
module TodoItem = {
  let component = ReasonReact.statelessComponent("TodoItem");
  let make = (~item, ~onToggle, _children) => {
    ...component,
    render: (_self) =>
      <div className="item" onClick=((_evt) => onToggle())>
        <input
          _type="checkbox"
          checked=Js.Boolean.to_js_boolean(item.completed)
        />
        (str(item.title))
      </div>
  }
};

/* Defines a text input box. Once user fills in text and presses
   enter, the text is added as a new Todo item */
module TextInput = {
  type state = string;
  let component = ReasonReact.reducerComponent("TextInput");
  let make = (~onSubmit, _children) => {
    ...component,
    initialState: () => "",
    reducer: (newText, _text) => ReasonReact.Update(newText),
    render: ({state: text, send}) =>
      <input
        value=text
        _type="text"
        placeholder="Plan out your day :)"
        onChange=((evt) => send(valueFromEvent(evt)))
        onKeyDown=((evt) => 
          if (ReactEventRe.Keyboard.key(evt) == "Enter") {
            onSubmit(text);
            send("")
          })
      />
  }
};

/* Defines type of component that TodoApp is */
let component = ReasonReact.reducerComponent("TodoApp");

/* Underscore before arg removes compiler warning if arg is not used in function */
let make = (_children) => {
  ...component,
  initialState: () => {
    items: []
  },

  /* Defines what happens when these actions are called */
  reducer: (action, {items}) => 
    switch action {
      | AddItem(text) => ReasonReact.Update({items: [newItem(text), ...items]})

      /* Map over list of items, find correct item by ID, flip completed boolean */
      | ToggleItem(id) => 
        let items = List.map((item) => 
          (item.id === id)? {...item, completed: !item.completed} : item, items
        );
        ReasonReact.Update({items: items})
    },

  render: ({state: {items}, send}) => {
    let numItems = items |> List.filter((item) => !item.completed) |> List.length;
    <div className="app">
      <div className="title"> 
        (str("Today's Todo List")) 
        <TextInput onSubmit=((text) => send(AddItem(text))) />
      </div>
      <div className="items"> 
        (ReasonReact.arrayToElement(Array.of_list(
          List.map((item) => <TodoItem 
            key=(string_of_int(item.id))
            onToggle=(() => send(ToggleItem(item.id)))
            item/>, items)
        )))
      </div>
      <div className="footer">
        (str(string_of_int(numItems) ++ if(numItems === 1) {" item left for today!"} else {" items left for today!"}))
      </div>
    </div>
  }
};


