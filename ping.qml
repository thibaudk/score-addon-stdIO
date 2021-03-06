import Ossia 1.0 as Ossia

Ossia.StdIO
{
    property string program: "ping"

    function onMessage(message) { // evaluated each time a message is received
        return [{address: "pings/stdout", value: message}]; // assign values to namespaces in the tree
    }

    function createTree() {
        return [
        {
            name: "pings",
            children: [
                {
                    name: "ossia.io",
                    type: Ossia.Type.Int,
                    write: "$name -c $val"
                },
                {
                    name: "stdout",
                    type: Ossia.Type.String
                },
                {
                    name: "killSwitch",
                    type: Ossia.Type.Impulse,
                    write: "$kill"
                }
            ]
        }
        ];
    }
}
