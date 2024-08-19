import tkinter as tk

class SampleWindow(tk.Tk):
    def __init__(self):
        super().__init__()

        # Window setup
        self.title("Sample Window")
        self.geometry("1024x768")

        # Create a menu
        menubar = tk.Menu(self)
        filemenu = tk.Menu(menubar, tearoff=0)
        filemenu.add_command(label="Exit", command=self.exit_program)
        menubar.add_cascade(label="File", menu=filemenu)
        self.config(menu=menubar)

        # Create the Exit button
        self.exit_button = tk.Button(self, text="Exit", command=self.exit_program)
        self.exit_button.pack(side=tk.BOTTOM, anchor=tk.SE, padx=10, pady=10)

        # Bind the resize event
        self.bind("<Configure>", self.on_resize)

        # Initial draw of columns and ball
        self.canvas = tk.Canvas(self, bg="white")
        self.canvas.pack(fill=tk.BOTH, expand=True)
        self.draw_columns_and_ball()

    def on_resize(self, event):
        # Redraw the columns and ball on resize
        self.draw_columns_and_ball()

    def draw_columns_and_ball(self):
        self.canvas.delete("all")
        
        # Get the current window size
        width = self.winfo_width()
        height = self.winfo_height()

        # Column and ball calculations
        num_columns = 8
        column_width = width // (2 * num_columns + 1)
        spacing = column_width
        column_bottom = height - 50

        # Draw the columns
        for i in range(num_columns):
            x = spacing * (i + 1) + column_width * i
            self.canvas.create_rectangle(x, 100, x + column_width, column_bottom, fill="darkgrey")

        # Draw the ball above the first space
        ball_x = spacing
        ball_y = 50
        self.canvas.create_oval(ball_x, ball_y - column_width, ball_x + column_width, ball_y, fill="blue")

    def exit_program(self):
        self.quit()

if __name__ == "__main__":
    app = SampleWindow()
    app.mainloop()
